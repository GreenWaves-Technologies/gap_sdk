import torch
import torch.nn as nn
import torchvision
import torchvision.transforms as transforms

# Device configuration
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
# device = 'cpu'
# Hyper-parameters 
# input_size = 784 # 28x28
num_classes = 10
num_epochs = 2
batch_size = 100
learning_rate = 0.001

input_size = 28
sequence_length = 28
hidden_size = 64
num_layers = 2

NN_MODE = 'gru'
BIDIR = True

# Export not supported for quantized training !!!
QUANT = False

# MNIST dataset 
train_dataset = torchvision.datasets.MNIST(root='./data', 
										   train=True, 
										   transform=transforms.ToTensor(),
										   download=True)

test_dataset = torchvision.datasets.MNIST(root='./data', 
										  train=False, 
										  transform=transforms.ToTensor())

# Data loader
train_loader = torch.utils.data.DataLoader(dataset=train_dataset, 
										   batch_size=batch_size, 
										   shuffle=True)

test_loader = torch.utils.data.DataLoader(dataset=test_dataset, 
										  batch_size=batch_size, 
										  shuffle=False)

# Fully connected neural network with one hidden layer
class RNN(nn.Module):
	def __init__(self, input_size, hidden_size, num_layers, num_classes, nn_mode='gru', quant=False, bidir=False):
		super(RNN, self).__init__()
		self.num_layers = num_layers
		self.hidden_size = hidden_size
		self.input_size = input_size
		self.nn_mode = nn_mode
		self.quant = quant
		self.bidir = bidir
		if quant:
			self.quant1 = torch.quantization.QuantStub()
			self.quant2 = torch.quantization.QuantStub()
			self.dequant1 = torch.quantization.DeQuantStub()
			self.dequant2 = torch.quantization.DeQuantStub()
		if nn_mode == 'rnn':
			self.rnn = nn.RNN(input_size, hidden_size, num_layers, batch_first=True, bidirectional=bidir)
			# -> x needs to be: (batch_size, seq, input_size)
		elif nn_mode == 'gru':
			self.gru = nn.GRU(input_size, hidden_size, num_layers, batch_first=True, bidirectional=bidir)
		elif nn_mode == 'lstm':
			self.lstm = nn.LSTM(input_size, hidden_size, num_layers, batch_first=True, bidirectional=bidir)

		self.fc = nn.Linear(hidden_size * 2 if bidir else 1, num_classes)
	
	def init_hidden(self, batch_size):
		if self.nn_mode == 'lstm':
			return (torch.zeros(self.num_layers * 2 if self.bidir else 1, batch_size, self.hidden_size).to(device),
					torch.zeros(self.num_layers * 2 if self.bidir else 1, batch_size, self.hidden_size).to(device))
		else:
			return torch.zeros(self.num_layers * 2 if self.bidir else 1, batch_size, self.hidden_size).to(device)

	def forward(self, x, hidden_state=None):
		# Set initial hidden states (and cell states for LSTM)
		if self.nn_mode == 'lstm':
			if hidden_state is None:
				h0, c0 = self.init_hidden(1)
			else:
				h0, c0 = hidden_state
		else:
			if hidden_state is None:
				h0 = self.init_hidden(1)
			else:
				h0 = hidden_state
		
		# Forward propagate RNN
		if self.quant:
			x = self.quant1(x)
		if self.nn_mode == 'rnn':
			out, _ = self.rnn(x, h0)
		elif self.nn_mode == 'gru':
			out, _ = self.gru(x, h0)
		else:
			out, _ = self.lstm(x, (h0,c0))  
		if self.quant:
			x = self.dequant1(x)
		
		# out: tensor of shape (batch_size, seq_length, hidden_size)
		# out: (n, 28, 128)
		
		# Decode the hidden state of the last time step
		out = out[:, -1, :]
		# out: (n, 128)
		 
		if self.quant:
			out = self.quant2(out)
		out = self.fc(out)
		if self.quant:
			out = self.dequant2(out)
		# out: (n, 10)
		return out

model = RNN(input_size, hidden_size, num_layers, num_classes, nn_mode=NN_MODE, quant=QUANT, bidir=BIDIR).to(device)
if QUANT:
	model.train()
	model.qconfig = torch.quantization.QConfig(activation=torch.quantization.MinMaxObserver.with_args(dtype=torch.qint8), weight=torch.quantization.default_observer.with_args(dtype=torch.qint8))
	# model_fused = torch.quantization.fuse_modules(model,[['conv', 'bn', 'relu']])
	model = torch.quantization.prepare_qat(model)
# Loss and optimizer
criterion = nn.CrossEntropyLoss()
optimizer = torch.optim.Adam(model.parameters(), lr=learning_rate)

# Train the model
n_total_steps = len(train_loader)
for epoch in range(num_epochs):
	for i, (images, labels) in enumerate(train_loader):
		# origin shape: [N, 1, 28, 28]
		# resized: [N, 28, 28]
		images = images.reshape(-1, sequence_length, input_size).to(device)
		labels = labels.to(device)
		
		# Forward pass
		hc = model.init_hidden(images.size(0))
		outputs = model(images, hc)
		loss = criterion(outputs, labels)
		
		# Backward and optimize
		optimizer.zero_grad()
		loss.backward()
		optimizer.step()
		
		if (i+1) % 100 == 0:
			print (f'Epoch [{epoch+1}/{num_epochs}], Step [{i+1}/{n_total_steps}], Loss: {loss.item():.4f}')
if QUANT:
	model.eval()
	model = torch.quantization.convert(model)

# Test the model
# In test phase, we don't need to compute gradients (for memory efficiency)
with torch.no_grad():
	n_correct = 0
	n_samples = 0
	for images, labels in test_loader:
		images = images.reshape(-1, sequence_length, input_size).to(device)
		labels = labels.to(device)
		hc = model.init_hidden(images.size(0))
		outputs = model(images, hc)
		# max returns (value ,index)
		_, predicted = torch.max(outputs.data, 1)
		n_samples += labels.size(0)
		n_correct += (predicted == labels).sum().item()

	acc = 100.0 * n_correct / n_samples
	print(f'Accuracy of the network on the 10000 test images: {acc} %')

from torch.autograd import Variable

def get_onnx_file():
	onnx_filename = './%s%s.onnx'%("b" if BIDIR else "", NN_MODE)
	x = torch.rand(1, sequence_length, input_size, requires_grad=False).to(device)
	# need to freeze the hidden state to one batch to avoid nasty things being generated
	torch_out = torch.onnx.export(model, x,
								  onnx_filename, export_params=True,
								  do_constant_folding=True, verbose=True)
# try to output onnx file, where error occurs
get_onnx_file()

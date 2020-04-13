#!/usr/bin/env python3


#
# Copyright (C) 2018 ETH Zurich and University of Bologna
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#


import io
import socket
import pickle
import threading
from twisted.internet import protocol
from plptest_utils import *
import gi
gi.require_version('Gtk', '3.0')

from twisted.internet import gtk3reactor
gtk3reactor.install()

from gi.repository import Gtk as gtk

from twisted.internet import reactor


class UiClient(protocol.Protocol):

    def __init__(self, app):
        super(UiClient, self).__init__()
        self.app = app
        self.pendingData = None

    def setTestsetIcon(self, test, config, icon):
      test.running[config.__str__()] = True
      if not test.hide and len(test.childs) == 0 and test.isActiveForConfig(config): test.button[config.__str__()].set_image(gtk.Image.new_from_icon_name(icon, 1))
      for child in test.childs:
          self.setTestsetIcon(child, config, icon)

    def clicked_play(self, widget, test, config):
        self.transport.write(pickle.dumps(RunTests([test.getFullName()], [config])))
        self.setTestsetIcon(test, config, 'image-loading')

    def button_press(self, widget, event):
      if event.button == 3:
        #make widget popup
        widget.popup(None, None, None, None, 0, gtk.get_current_event_time())
        pass

    def view_log(self, menu_item, test, config):

      if test.log.get(config) != None:
        window = gtk.ApplicationWindow(application=self.app)

        scrolledwindow = gtk.ScrolledWindow()
        scrolledwindow.set_policy(gtk.PolicyType.AUTOMATIC, gtk.PolicyType.AUTOMATIC)
        window.add(scrolledwindow)

        label = gtk.Label(test.log[config])
        label.set_line_wrap(True)
        scrolledwindow.add(label)

        window.set_size_request(400, 300)
        window.show_all()

    def list_click(self, widget, event, test):
      test.expanded = not test.expanded

      self.test_expand([test], not test.expanded)

      for i in range(len(self.configs) + 1, -1, -1):
        self.grid.remove_column(i)
      
      for test in self.tests:
        self.dumpTestsetToTree(self.grid, self.configs, test)
      self.window.show_all()

      return

      test.walk_cb(self.test_hide)
      self.window.show_all()


      #button.hide()

    def test_expand(self, tests, hide):
      for test in tests:
        if len(test.childs) == 0: continue
        else:
          hide = hide or not test.expanded
          for child in test.childs:
            child.hide = hide 
            self.test_expand([child], hide)

    def test_hide2(self, test, status):
      test.hide = not status

    def test_init(self, test):
      test.hide = False
      test.expanded = False
      test.button = {}
      test.log = {}
      test.running = {}
      test.store = {}

    def test_hide(self, test):

      if not test.isActive: return

      for i in range(test.lastRow-1, test.firstRow, -1):
        self.grid.remove_row(i)


    def dumpTestsetToTree(self, grid, configs, testset, index=0, indent=''):

      if not testset.isActive or testset.hide: return None

      testset.firstRow = index

      label = gtk.Label(halign=gtk.Align.START)
      label.set_text(indent + testset.name)
      label.set_justify(gtk.Justification.LEFT)
      if len(testset.childs) != 0:
        img = gtk.Image.new_from_icon_name("list-add", 1)
        button = gtk.Button(image=img)
        button.props.relief = gtk.ReliefStyle.NONE
        button.connect("button-press-event", self.list_click, testset)
        grid.attach(button, 0, index, 1, 1)
      grid.attach(label, 1, index, 1, 1)

      configId = 0
      for config in configs:
        if not testset.isActiveForConfig(config): 
          configId += 1
          continue
        if len(testset.childs) == 0:
          if testset.running.get(config.__str__()) == True: img = gtk.Image.new_from_icon_name('image-loading', 1)
          elif testset.status.get(config.__str__()) != None: img = gtk.Image.new_from_icon_name('emblem-default', 3) if testset.status.get(config.__str__()) else gtk.Image.new_from_icon_name('process-stop', 3)
          else: img = gtk.Image.new_from_icon_name("media-playback-start", 3)
          button = gtk.Button(image=img)
          menu = gtk.Menu()
          menu_item = gtk.MenuItem("Log")
          menu.append(menu_item)
          menu_item.show()
          menu_item.connect("activate", self.view_log, testset, config)
          button.connect_object("button-press-event", self.button_press, menu)
        else:
          button = gtk.Button(label="%d/%d" % (testset.getNbSuccess(config.__str__()), testset.getNbTests(config.__str__())))
        testset.button[config.__str__()] = button
        button.connect("clicked", self.clicked_play, testset, config)
        grid.attach(button, 2 + configId, index, 1, 1)
        configId += 1
      #img = gtk.Image.new_from_icon_name("media-playback-stop", 2)
      #button = gtk.Button(image=img)
      #grid.attach(button, 3, index, 1, 1)
      index += 1
      for child in testset.childs:
          newIndex = self.dumpTestsetToTree(grid, configs, child, index, indent + '    ')
          if newIndex != None: index = newIndex


      testset.lastRow = index


      return index

    def delete_event(self, widget, event, data=None):
        reactor.stop()
        return False


    def connectionMade(self):
        self.transport.write(pickle.dumps(GetTests()))


    def update_testsets(self, test, config):
      if not test.hide and len(test.childs) != 0:
        test.button[config.__str__()].set_label("%d/%d" % (test.getNbSuccess(config.__str__()), test.getNbTests(config.__str__())))

    def dataReceived(self, data):

      if self.pendingData != None:
        data = self.pendingData + data
        self.pendingData = None

      size = len(data)
      stream = io.BytesIO(data)

      while True:

        try:
          start = stream.tell()
          cmd = pickle.load(stream)
        except:
          if start != size: self.pendingData = data[start:]
          break

        if cmd.name == 'get tests':

          self.configs = cmd.configs
          self.tests = cmd.tests

          self.configIndex = {}
          for configId in range(0, len(cmd.configs)):
            self.configIndex[cmd.configs[configId].__str__()] = configId

          for test in self.tests:
            test.walk_cb(self.test_init)

          expanded = True

          for test in self.tests:
            test.expanded = expanded

          self.test_expand(self.tests, not expanded)

          self.window = gtk.ApplicationWindow(application=self.app)
          self.window.connect("delete_event", self.delete_event)


          self.window.set_title("Basic TreeView Example")


          #self.window.set_size_request(200, 200)
          self.window.set_size_request(-1, 600)
          #self.window.set_default_size()

          box = gtk.Box()

          scroll = gtk.ScrolledWindow()
          box.pack_start(scroll, True, True, 0)

          vbox = gtk.Box(orientation=gtk.Orientation.VERTICAL)
          box.pack_start(vbox, True, True, 0)

          confbox = gtk.Box(orientation=gtk.Orientation.VERTICAL)
          vbox.pack_start(confbox, True, True, 0)


          store = gtk.ListStore(str, int)
          tree = gtk.TreeView(store)
          renderer = gtk.CellRendererText()
          renderer2 = gtk.CellRendererText()
          column = gtk.TreeViewColumn("Config", renderer, text=0)
          tree.append_column(column)
          column = gtk.TreeViewColumn("Index", renderer2, text=1)
          tree.append_column(column)
          confbox.pack_start(tree, True, True, 0)
          for configId in range(0, len(cmd.configs)):
            store.append([cmd.configs[configId].__str__(), configId])



          testsbox = gtk.Box()
          vbox.pack_start(testsbox, True, True, 0)

          store = gtk.ListStore(str, int)
          self.store = store
          tree = gtk.TreeView(store)
          renderer = gtk.CellRendererText()
          renderer2 = gtk.CellRendererText()
          column = gtk.TreeViewColumn("Running tests", renderer, text=0)
          tree.append_column(column)
          column = gtk.TreeViewColumn("Config", renderer2, text=1)
          tree.append_column(column)
          testsbox.pack_start(tree, True, True, 0)



          self.window.add(box)
          viewport = gtk.Viewport()
          scroll.set_policy(gtk.PolicyType.NEVER, gtk.PolicyType.AUTOMATIC)
          scroll.add(viewport)

          grid = gtk.Grid()
          viewport.add(grid)
          self.grid = grid

          for test in cmd.tests:
            self.dumpTestsetToTree(grid, cmd.configs, test)

          self.window.show_all()

        elif cmd.name == "test run result":
          icon = 'emblem-default' if cmd.status else 'process-stop'
          for test_obj in self.tests:
            test = test_obj.get(cmd.test)
            if test is not None:
              test.running[cmd.config.__str__()] = False
              test.log[cmd.config] = cmd.log
              test.status[cmd.config.__str__()] = cmd.status
              self.store.remove(test.store[cmd.config.__str__()])
              if not test.hide:
                test.button[cmd.config.__str__()].set_image(gtk.Image.new_from_icon_name(icon, 3))
              test.walk_cb(self.update_testsets, down=False, config=cmd.config)

        elif cmd.name == 'test running':
          for test_obj in self.tests:
            test = test_obj.get(cmd.test)

            if test is not None:
              self.configIndex[cmd.config.__str__()]
              test.store[cmd.config.__str__()] = self.store.append([test.getFullName(), self.configIndex[cmd.config.__str__()]])
    
    def connectionLost(self, reason):
        print("connection lost")

class UiFactory(protocol.ClientFactory):
    protocol = UiClient

    def buildProtocol(self, addr):
        return UiClient(self.app)

    def __init__(self, app):
        super(UiFactory, self).__init__()
        self.app = app

    def clientConnectionFailed(self, connector, reason):
        print("Connection failed - goodbye!")
        #reactor.stop()
    
    def clientConnectionLost(self, connector, reason):
        print("Connection lost - goodbye!")
        #reactor.stop()

class SockThread (threading.Thread):
  def __init__(self):
    threading.Thread.__init__(self)

  def run(self):
    reactor.run()

def app_activate(self):
  pass



class TestGui(object):

  def __init__(self):
    app = gtk.Application()
    app.connect("activate", app_activate)
    reactor.registerGApplication(app)
    f = UiFactory(app)
    reactor.connectTCP("localhost", 38497, f)

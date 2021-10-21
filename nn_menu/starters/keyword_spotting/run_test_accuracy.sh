make test_accuracy SMALL=1  USE_HIGH_PREC=0 USE_POWER=1 &> accuracy_log/log_test_small_power_v2.txt
make test_accuracy SMALL=1  USE_HIGH_PREC=1 USE_POWER=0 &> accuracy_log/log_test_small_spectr_v2.txt
make test_accuracy SMALL=1  USE_HIGH_PREC=0 USE_POWER=1 &> accuracy_log/log_test_small_power_v2.txt
make test_accuracy SMALL=1  USE_HIGH_PREC=1 USE_POWER=0 &> accuracy_log/log_test_small_spectr_v2.txt

make test_accuracy MEDIUM=1 USE_HIGH_PREC=0 USE_POWER=1 &> accuracy_log/log_test_medium_power_v2.txt
make test_accuracy MEDIUM=1 USE_HIGH_PREC=1 USE_POWER=0 &> accuracy_log/log_test_medium_spectr_v2.txt
make test_accuracy MEDIUM=1 USE_HIGH_PREC=0 USE_POWER=1 &> accuracy_log/log_test_medium_power_v2.txt
make test_accuracy MEDIUM=1 USE_HIGH_PREC=1 USE_POWER=0 &> accuracy_log/log_test_medium_spectr_v2.txt

make test_accuracy LARGE=1  USE_HIGH_PREC=0 USE_POWER=1 &> accuracy_log/log_test_large_power_v2.txt
make test_accuracy LARGE=1  USE_HIGH_PREC=1 USE_POWER=1 &> accuracy_log/log_test_large_hp_power_v2.txt
make test_accuracy LARGE=1  USE_HIGH_PREC=0 USE_POWER=0 &> accuracy_log/log_test_large_spectr_v2.txt
make test_accuracy LARGE=1  USE_HIGH_PREC=1 USE_POWER=0 &> accuracy_log/log_test_large_hp_spectr_v2.txt
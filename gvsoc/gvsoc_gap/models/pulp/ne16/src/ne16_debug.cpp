/*
 * Copyright (C) 2020  GreenWaves Technologies, SAS
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Authors: Francesco Conti, University of Bologna & GreenWaves Technologies (f.conti@unibo.it)
 */

#include <ne16.hpp>

void Ne16::debug_x_buffer() {
    if(this->mode_linear) {
        printf ("================ X_BUFFER LINEAR MODE TRACES ================\n");
        for (auto i=0; i<32; i++) {
            printf ("{");
            for (auto k=0; k<16; k++) {
                printf (" %3x ", x_buffer_linear[i+k*32]);
            }
            printf ("}\n");
        }
    }
    else {
        printf ("================ X_BUFFER TRACES ================\n");
        printf ("X_buffer [5][5][16]: \n");
        for (auto k=0; k<this->F_BUFFER_SIZE; k++) {
            for (auto j=0; j<this->F_BUFFER_SIZE; j++) {
                printf ("{");
                for (auto i=0; i<this->TP_IN; i++) {
                    printf (" %3x ", x_buffer[i+j*this->TP_IN+k*this->F_BUFFER_SIZE*this->TP_IN]);
                }
                printf ("} \n");
            }
            printf ("\n");
        }
    }
}

void Ne16::debug_x_array() {
    printf ("================ X_ARRAY TRACES ================\n");
    printf ("X_array [9][9][16]: \n");
    for (auto k=0; k<this->NR_COLUMN; k++) {
        for (auto j=0; j<this->COLUMN_SIZE; j++) {
            printf ("{");
            for (auto i=0; i<this->TP_IN; i++) {
                printf (" %3x ", x_array[i+j*this->TP_IN+k*this->COLUMN_SIZE*this->TP_IN]);
            }
            printf ("} \n");
        }
        printf ("\n");
    }
}

void Ne16::debug_accum(){
    printf ("================ ACCUMULATORS DEBUG TRACES ================\n");
    printf ("accum[9][32]: \n");
    for (auto j=0; j<this->TP_OUT; j++) {
        printf ("{");
        for (auto i=0; i<this->COLUMN_SIZE; i++) {
            printf (" %8lx ", accum[i+j*this->COLUMN_SIZE]);
        }
        printf ("} \n");
    }
    printf ("} \n");
}

// void Ne16::debug_psum_column(){
//   std::ostringstream stringStream;
//   stringStream << "psum_column[9] = \n" << (this->trace_format?std::hex:std::dec) << std::setw(8) << xt::cast<int32_t>(this->psum_column) << std::dec << "\n";
//   std::string copyOfStr = stringStream.str();
//   this->trace.msg(vp::trace::LEVEL_DEBUG, copyOfStr.c_str());
// }

void Ne16::debug_psum_block(){
    printf ("================ PSUM BLOCK DEBUG TRACES ================\n");
    printf ("psum_block[9][9]: \n");
    for (auto j=0; j<this->COLUMN_SIZE; j++) {
        printf ("{");
        for (auto i=0; i<this->NR_COLUMN; i++) {
            printf (" %5lx,", psum_block[i+this->NR_COLUMN*j]);
        }
        printf ("} \n");
    }
    printf ("} \n");
}

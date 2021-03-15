/*
 * Copyright (C) 2020 GreenWaves Technologies, SAS, ETH Zurich and
 *                    University of Bologna
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

#include "nina_b112.hpp"

#include <iostream>
#include <regex>
#include <vector>

namespace {

    std::string wrap_response(std::string response, std::string code)
    {
        if (!response.empty())
        {
            return std::string("\r\n") + response +
                   std::string("\r\n") + code +
                   std::string("\r\n");
        }
        return std::string("\r\n") + code + std::string("\r\n");
    }

    std::string ok_response(std::string response)
    {
        return ::wrap_response(response, std::string("OK"));
    }

    std::string error_response(std::string response)
    {
        return ::wrap_response(response, std::string("ERROR"));
    }

    std::vector<std::string> split(const std::string str, const std::string delimiter)
    {
        std::regex delimiter_regex(delimiter);
        std::vector<std::string> list(std::sregex_token_iterator(str.begin(), str.end(), delimiter_regex, -1),
                                      std::sregex_token_iterator());
        return list;
    }
}

std::string Nina_b112::parse_cmd(std::string input_cmd)
{
    std::smatch match;

    /* empty command */
    if (input_cmd.empty())
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Received Attention_AT command\n");
        return ::ok_response("");
    }

    /* echo On/Off command */
    std::regex cmd_echo_mode("E[01]");
    if(std::regex_match(input_cmd, match, cmd_echo_mode))
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Received echo_on_off command\n");
        //TODO
        return ok_response("");
    }

    /* factory reset command */
    /* Reboot required */
    std::regex cmd_factory_reset("\\+UFACTORY");
    if(std::regex_match(input_cmd, match, cmd_factory_reset))
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Received factory_reset command (reboot required)\n");
        //TODO
        return ok_response("");
    }

    /* operating mode change command */
    std::regex cmd_op_mode_data("O[012]?");
    if(std::regex_match(input_cmd, match, cmd_op_mode_data))
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Received enter_data_mode command\n");
        std::string cmd = match[0];
        if (cmd.back() == 2)
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Switching to extended data operating mode: NOT SUPPORTED\n");
        }
        else if (cmd.back() == 0)
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Switching to command mode\n");
            this->behavior.operating_mode = NINA_B112_OPERATING_MODE_COMMAND;
        }
        else
        {
            /* default */
            this->trace.msg(vp::trace::LEVEL_INFO, "Switching to data operating mode\n");
            this->behavior.operating_mode = NINA_B112_OPERATING_MODE_DATA;
        }
        return ok_response("");
    }

    /* unbond command */
    std::regex cmd_unbond("\\+UBTUB\\=.*");
    if(std::regex_match(input_cmd, match, cmd_unbond))
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Received unbond command\n");
        //TODO
        return ok_response("");
    }

    /* bluetooth low energy role command */
    std::regex cmd_ble_role("\\+UBTLE\\=[0123]");
    if(std::regex_match(input_cmd, match, cmd_ble_role))
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Received bluetooth_low_energy_role command\n");
        //TODO
        return ok_response("");
    }

    /* local name command */
    std::regex cmd_local_name("\\+UBTLN\\=.*");
    if(std::regex_match(input_cmd, match, cmd_local_name))
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Received local_name command\n");
        //TODO
        //TODO parse string (surrounded by double quotes ? not mandatory according to real module),
        //verify length (max 31 characters)
        return ok_response("");
    }

    /* local name read command */
    std::regex cmd_local_name_read("\\+UBTLN\\?");
    if(std::regex_match(input_cmd, match, cmd_local_name_read))
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Received local_name_read command\n");
        return ok_response(std::string("+UBTLN:\"") + this->behavior.local_name + std::string("\""));
    }

    /* RS232 parameters command */
    std::regex cmd_rs232_params("\\+UMRS\\=.*");
    if(std::regex_match(input_cmd, match, cmd_rs232_params))
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Received rs232_parameters command\n");

        input_cmd.erase(0, 6);

        std::vector<std::string> args = ::split(input_cmd, ",");

        nina_b112_uart_cfg_t new_cfg = this->uart_cfg;

        /* baudrate */
        if (args.size() >= 1)
        {
            std::vector<std::string> supported_baudrates{
                "19200",
                "38400",
                "57600",
                "115200",
                "230400",
                "460800",
                "1000000",
            };

            bool found = false;
            for(std::string s: supported_baudrates)
            {
                if (0 == s.compare(args[0]))
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                this->trace.msg(vp::trace::LEVEL_ERROR, "Baudrate %s is not supported\n", args[0].c_str());
                return error_response("");
            }

            new_cfg.baudrate = std::stoi(args[0]);
        }

        /* flow_control */
        if (args.size() >= 2)
        {
            if(0 == args[1].compare("1"))
            {
                new_cfg.flow_control = true;
            }
            else if (0 == args[1].compare("2"))
            {
                new_cfg.flow_control = false;
            }
            else
            {
                this->trace.msg(vp::trace::LEVEL_ERROR, "flow_control argument %s is not supported\n", args[1].c_str());
                return error_response("");
            }
        }

        /* data_bits */
        if (args.size() >= 3)
        {
            /* Only 8 bits support ? */
            if (0 == args[2].compare("8"))
            {
                new_cfg.data_bits = 8;
            }
            else
            {
                this->trace.msg(vp::trace::LEVEL_ERROR, "data_bits argument %s is not supported\n", args[2].c_str());
                return error_response("");
            }
        }

        /* stop_bits */
        if (args.size() >= 4)
        {
            if (0 == args[3].compare("1"))
            {
                new_cfg.stop_bits = 1;
            }
            else
            {
                this->trace.msg(vp::trace::LEVEL_ERROR, "stop_bits argument %s is not supported\n", args[3].c_str());
                return error_response("");
            }
        }

        /* parity */
        if (args.size() >= 5)
        {
            if (0 == args[4].compare("1"))
            {
                new_cfg.parity = NINA_B112_UART_PARITY_NONE;
            }
            else if (0 == args[4].compare("3"))
            {
                new_cfg.parity = NINA_B112_UART_PARITY_EVEN;
            }
            else
            {
                this->trace.msg(vp::trace::LEVEL_ERROR, "parity argument %s is not supported\n", args[4].c_str());
                return error_response("");
            }
        }

        /* change_after_confirm */
        bool change_now = false;
        if (args.size() >= 6)
        {
            if (0 == args[5].compare("0"))
            {
                change_now = false;
            }
            else if (0 == args[5].compare("1"))
            {
                change_now = true;
            }
            else
            {
                this->trace.msg(vp::trace::LEVEL_ERROR, "change_after_confirm argument %s is not supported\n", args[5].c_str());
                return error_response("");
            }
        }

        if (change_now)
        {
            this->trace.msg(vp::trace::LEVEL_INFO, "Applying new uart configuration:\n");
            this->trace.msg(vp::trace::LEVEL_INFO, "- baudrate: %d\n", new_cfg.baudrate);
            this->trace.msg(vp::trace::LEVEL_INFO, "- data_bits: %d\n", new_cfg.data_bits);
            this->trace.msg(vp::trace::LEVEL_INFO, "- stop_bits: %d\n", new_cfg.stop_bits);
            this->trace.msg(vp::trace::LEVEL_INFO, "- parity: %s\n",
                    (new_cfg.parity == NINA_B112_UART_PARITY_NONE) ? "none" : "even");
            this->trace.msg(vp::trace::LEVEL_INFO, "- flow_control: %s\n",
                    (new_cfg.flow_control) ? "enabled" : "disabled");

            this->uart_cfg = new_cfg;
        }

        return ok_response("");
    }

    /* RS232 parameters read command */
    std::regex cmd_rs232_params_read("\\+UMRS\\?");
    if(std::regex_match(input_cmd, match, cmd_rs232_params_read))
    {
        this->trace.msg(vp::trace::LEVEL_INFO, "Received rs232_parameters_read command\n");

        std::ostringstream stream;

        stream << "+URMS:";

        /* baudrate */
        stream << this->uart_cfg.baudrate << ",";

        /* flow_control 1: used, 2: not_used */
        stream << ((this->uart_cfg.flow_control) ? "1" : "2") << ",";

        /* data_bits */
        stream << this->uart_cfg.data_bits << ",";

        /* stop_bits */
        stream << this->uart_cfg.stop_bits << ",";

        /* parity 1: none, 2: odd, 3:even */
        switch(this->uart_cfg.parity)
        {
            case NINA_B112_UART_PARITY_EVEN:
                stream << "3";
                break;
            default:
                stream << "1";
                break;
        }

        return ok_response(stream.str());
    }

    /* Commands to support:
     *
     * CFG_SAVE        (&W      )
     * DEVICE_REBOOT   (+CPWROFF)
     * MANUFACTURER_ID (+CGMI   )
     * MODEL_ID        (+CGMM   )
     * SW_VERSION_ID   (+CGMR   )
     * SERIAL_NUMBER   (+CGSN   )
     * BOND            (+UBTB   )
     * UNBOND          (+UBTUB  )
     * PEER_CONNECT    (+UDCP   )
     * PEER_DISCONNECT (+UDCPC  )
     * DEVICE_ROLE_SET (+UBTLE  )
     * DEVICE_ROLE_GET (+UBTLE? )
     */

    this->trace.msg(vp::trace::LEVEL_INFO, "Received unknown command %s\n", input_cmd.c_str());
    return error_response("");
}

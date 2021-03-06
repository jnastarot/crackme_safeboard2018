#include "stdafx.h"


void get_email_hash(const char * email, uint32_t email_len, uint32_t& magic_1, uint32_t& magic_2) {


    {
        magic_1 = 0x4552534B;//'ERSK'
        magic_2 = 0x4B415350;//'KASP'

        uint32_t current_email_ch_idx = 0;

        if (email_len > 0) {

            do {
                uint64_t magic_pair = uint64_t(magic_2) << 32 | magic_1;
                uint32_t email_hash_1 = (unsigned __int8)email[current_email_ch_idx] + magic_1;
                uint32_t email_hash_2 = ((unsigned __int8)email[current_email_ch_idx++] + magic_pair) >> 32;
                uint64_t email_magic_pair = uint64_t(email_hash_2) << 32 | email_hash_1;

                magic_2 = email_magic_pair >> 28;
                magic_1 = 0x10 * email_hash_1;

            } while (current_email_ch_idx < email_len);

        }
    }
}


bool check_key(const char * email, uint32_t email_len, uint8_t * serial) {


    for (int serial_idx = 0; serial_idx < 10; serial_idx++) { //check serial format
        if (!serial[serial_idx]) {
            return false;
        }
    }

    uint32_t magic_1;
    uint32_t magic_2;

    get_email_hash(email, email_len, magic_1, magic_2);


    serial[0] ^= serial[9];

    if (*(uint32_t *)serial == magic_1 && *((uint32_t *)serial + 1) == magic_2) {
        return true;
    }

    return false;
}


void generate_key(const char * email, uint32_t email_len, uint8_t * serial) {


    uint32_t magic_1;
    uint32_t magic_2;

    get_email_hash(email, email_len, magic_1, magic_2);


    *(uint32_t*)(&serial[0]) = magic_1;
    *(uint32_t*)(&serial[4]) = magic_2;
    *(uint32_t*)(&serial[8]) = rand(); //not used byte



    if (serial[0] == 1) {
        serial[0] ^= 2;
        serial[9] = 2;
    }
    else {
        serial[0] ^= 1;
        serial[9] = 1;
    }
}

int main() {


    printf(
        "*    Writed and reversed by JNA\n" \
        "* [G] - generate key                                     \n" \
        "* [C] - check key                                        \n" \
        "* [E or Q] - exit                                        \n\n"
    );

    std::string command;

    while (cin >> command) {

        if (!command.size()) { continue; }

        switch (command[0]) {

        case 'G': case 'g': {
            std::string email;

            cout << "Email: ";  cin >> email;
            
            uint8_t serial[10];
            generate_key(email.c_str(), email.size(), serial);

            for (uint8_t s_idx = 0; s_idx < 10;s_idx++) {
                printf("%02x", serial[s_idx]);
            }

            cout << endl;

            break;
        }

        case 'C': case 'c': {
            std::string email;
            std::string key;

            cout << "Email: ";  cin >> email;
            cout << "Key: ";  cin >> key;

            uint8_t serial[10];
            memset(serial, 0, 10);

            if (key.size() != 20) {
                cout << "Invalid key!" << endl;
                continue;
            }

            for (uint8_t s_idx = 0; s_idx < key.size(); s_idx++) {

                if (key[s_idx] >= 'a' && key[s_idx] <= 'f') {
                    serial[s_idx / 2] = serial[s_idx / 2] | ( (key[s_idx] - 'a' + 10) << ( (s_idx & 1) ? 0 : 4));
                }
                else if (key[s_idx] >= 'A' && key[s_idx] <= 'F') {
                    serial[s_idx / 2] = serial[s_idx / 2] | ((key[s_idx] - 'F' + 10) << ((s_idx & 1) ? 0 : 4));
                }
                else if (key[s_idx] >= '0' && key[s_idx] <= '9') {
                    serial[s_idx / 2] = serial[s_idx / 2] | ((key[s_idx] - '0') << ((s_idx & 1) ? 0 : 4));
                }
            }


            if (check_key(email.c_str(), email.size(), serial)) {
                cout << "Valid key!" << endl;
            }
            else {
                cout << "Invalid key!" << endl;
            }

            break;
        }


        case 'E':case 'e':case 'Q':case 'q': { 
            return 0; 
        }
        }
    }





  //  generate_key("jnastarot@yandex.ru", sizeof("jnastarot@yandex.ru") - 1, serial_);
  //  check_key("jnastarot@yandex.ru", sizeof("jnastarot@yandex.ru") - 1, serial_);

    

    return 0;
}


/*** 
 * @Author: nexisato
 * @Date: 2024-02-27 08:43:40
 * @FilePath: /miracl_validation/test/test_acc.cpp
 * @Description: 
 * @
 */
#include "accumulator.h"
#include <jsoncpp/json/json.h>

extern miracl *mip;

void get_pid_from_json(const char *&path, std::vector<Big> &pids) {
    std::ifstream ifs(path);
    Json::Reader reader;  // Declare and initialize the 'ifs' variable
    Json::Value root;
    Json::Value item;
    if (!reader.parse(ifs, root, false)) {
        std::cerr << "parse error" << std::endl;
        return;
    }
    const int size = root["packages"].size();
    mip->IOBASE = 16;
    for (int i = 0; i < size; ++i) {
        item = root["packages"][i];

        char *pid_str = const_cast<char*>(item["pid"].asCString());
        utils::toUpperCase(pid_str);

        Big pid_val(pid_str);
        pids.push_back(pid_val);
    }
}

int main() {

    // 1. system_setup
    Accumulator *acc_ptr = new Accumulator(_KEY_LEN_);
    //acc_ptr->print_params();

    // 2. add_member
    std::vector<Big> pids;
    const char *json_path = "../assets.json";
    get_pid_from_json(json_path, pids);


    auto add_start = std::chrono::system_clock::now();
    
    for (const auto &pid : pids) {
        acc_ptr->add_member(pid);
    }
    acc_ptr->witness_generate_all();

    auto add_end = std::chrono::system_clock::now();
    std::cout << "[Timing]Register Time: " << utils::count_time(add_start, add_end)
              << " ms" << std::endl;
    
    std::cout << "acc_cur: " << acc_ptr->acc_cur << std::endl;
    std::cout << "N (public key): " << acc_ptr->public_key << std::endl;
    // 3. verify_raw
    const int test_time = 1;
    auto verify_start = std::chrono::system_clock::now();

    for (int cnt = 0; cnt < test_time; ++cnt) {
        for (size_t i = 0; i < pids.size(); ++i) {
            if (acc_ptr->verify_member(acc_ptr->wits[i], pids[i])) {
                //std::cout << "verify success" << std::endl;
                std::cout << "pid[" << i << "]: " << pids[i] << std::endl;
                std::cout << "wit[" << i << "]: " << acc_ptr->wits[i] << std::endl;
            }
            else {
                throw std::runtime_error("verify failed");
            }
        }
    }

    auto verify_end = std::chrono::system_clock::now();

    std::cout << "[Timing]Verify Time: " << utils::count_time(verify_start, verify_end) / (test_time * pids.size())
              << " ms" << std::endl;


    // // 4. verify_powd
    // int order = 64;
    // auto pow_verify_start = std::chrono::system_clock::now();

    // for (int cnt = 0; cnt < test_time; ++cnt) {
    //     for (size_t i = 0; i < pids.size(); ++i) {
    //         if (acc_ptr->verify_member_hash(acc_ptr->wits[i], pids[i], order))
    //             std::cout << "verify success" << std::endl;
    //         else
    //             std::cout << "verify failed" << std::endl;
    //     }
    // }

    // auto pow_verify_end = std::chrono::system_clock::now();

    // std::cout << "[Timing]Verify-Pown Time: " << utils::count_time(pow_verify_start, pow_verify_end) / (test_time * pids.size())
    //           << " ms" << std::endl;


    // // remove member
    // std::vector<Big> remove_pids;
    // remove_pids.push_back(pids[0]);
    // remove_pids.push_back(pids[1]);

    // for (auto &pid : remove_pids) {
    //     std::cout << "remove pid: " << pid << std::endl;
    // }
    // Big aux = acc_ptr->remove_members(remove_pids);
    // acc_ptr->update_witness(aux);

    // std::cout << "Current Wits.size(): " << acc_ptr->wits.size() << std::endl;
    // std::cout << "====================" << std::endl;


    // // 5. reverify
    // auto reverify_start = std::chrono::system_clock::now();


    // for (int cnt = 0; cnt < test_time; ++cnt) {
    //     for (size_t i = 0; i < acc_ptr->wits.size(); ++i) {
    //         if (acc_ptr->verify_member(acc_ptr->wits[i], pids[i + 2]))
    //             std::cout << "verify success" << std::endl;
    //         else
    //             std::cout << "verify failed" << std::endl;
    //     }
    // }


    // auto reverify_end = std::chrono::system_clock::now();
    // std::cout << "[Timing]Reverify Time: " << utils::count_time(reverify_start, reverify_end)
    //           << " ms" << std::endl;





    return 0;
}
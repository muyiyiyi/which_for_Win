//
// Created by muyi on 2023/9/3.
//

#ifndef WHICH_FOR_WIN_UTILS_H
#define WHICH_FOR_WIN_UTILS_H
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <experimental/filesystem>
using namespace std;
namespace fs = experimental::filesystem;
/*!
 * 判断字符串cmd是否匹配路径f_path f_path是一个文件的路径
 * @param cmd
 * @param f_path
 * @param match_fz
 * @return
 */
bool isMatch(string cmd, fs::path& f_path, bool match_fz)
{
    bool matched = false;
    string basename = f_path.stem().string();  // 文件名 不带后缀

    if (match_fz) {  // 模糊匹配的话 包含即可
        if (basename.find(cmd) != string::npos){
            matched = true;
        }
    } else{
        if (basename == cmd){
            matched = true;
        }
    }
    return matched;
}

/*!
 * 在路径f_path下寻找cmd的文件 f_path可能是文件夹可能是文件
 * @param cmd
 * @param f_path
 * @param match_fz
 * @param results
 */
void find_cmd_in_f_path(string cmd, fs::path& f_path, bool match_fz, vector<string>& results){
    if (fs::is_regular_file(f_path)){  //如果是正常文件
        if (isMatch(cmd, f_path,match_fz))
        {
            results.push_back(f_path.string());
        }
    }
    else if (fs::is_directory(f_path)){
        for (auto& entry: fs::directory_iterator (f_path)){  //遍历文件夹的子项
            auto sub_path = entry.path();
            // 如果子项是文件 比较是否是要查找的
            if(fs::is_regular_file(sub_path))
            {
                if (isMatch(cmd, sub_path, match_fz)){
                    results.push_back(sub_path.string());
                }
            }
        }
    }
}


/*!
 * 在环境变量的路径中查找某个指令文件cmd
 * @param cmd
 * @param match_fz 模糊匹配，包含即可，大小写不敏感
 * @return
 */
void find_cmd_in_envpath(string cmd, bool match_fz, vector<string>& find_results)
{
    char** env = environ;  // 系统的环境变量数组
    vector<string> visited_dirs; // 记录已经查找过的文件夹
    while(*env != 0) {
        string cur_str_env = *env; //形如 USERNAME=xxx;xxx的环境变量字符串
        env ++;
        // 按照=拆分 获取环境变量的全部取值
        string env_values;

        stringstream ss(cur_str_env);
        getline(ss,env_values,'=');
        getline(ss,env_values,'=');

        if (env_values.find(';') == string::npos){
            if (std::find(visited_dirs.begin(), visited_dirs.end(), env_values) == visited_dirs.end())
            {
                fs::path f_path(env_values);
                visited_dirs.push_back(env_values);
                if (fs::exists(f_path)) {  // 环境变量只有一个值 不需要分割 直接判断 如果路径存在
                    find_cmd_in_f_path(cmd, f_path,match_fz, find_results);
                }
            }

        }else{
            ss.clear(); // clear只是清空状态 需要结合ss.str(newstr)使用
            ss.str(env_values); // 清空字符串流 重置为带=的全部的字符串
            string env_value; // 按照分号;分割的环境变量取值
            while (getline(ss, env_value, ';')){  // 循环处理每一个分割的结果 env_value就是取出的值

                if (std::find(visited_dirs.begin(), visited_dirs.end(), env_value) == visited_dirs.end()){
                    fs::path f_path(env_value);
                    if (fs::exists(f_path)){  // 如果环境变量值是一个存在的路径
                        find_cmd_in_f_path(cmd, f_path,match_fz, find_results);
                    }
                    visited_dirs.push_back(env_value);
                }
            }
        }
    }

    cout << "search all env path finished, valid number of env dirs = " << visited_dirs.size() << endl;
}

#endif //WHICH_FOR_WIN_UTILS_H

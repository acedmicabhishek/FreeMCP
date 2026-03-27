
#include <string>
#include <cstdlib>
#include <cstdio>
#include <array>
#include <memory>

extern "C" void log_message(const char* msg);

namespace FreeMCP::Module::Files {

static std::string run(const std::string& cmd) {
    std::string result;
    std::array<char, 512> buffer;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) return "[Error]";
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        result += buffer.data();
    while (!result.empty() && (result.back() == '\n' || result.back() == ' '))
        result.pop_back();
    return result.empty() ? "[Done]" : result;
}


std::string list(const std::string& path)    { return run("ls -la " + (path.empty() ? "." : path) + " 2>&1"); }
std::string tree(const std::string& path)    { return run("tree -L 2 " + (path.empty() ? "." : path) + " 2>&1 | head -30"); }
std::string pwd()                            { return run("pwd"); }
std::string size(const std::string& path)    { return run("du -sh " + path + " 2>&1"); }


std::string create_file(const std::string& path)   { return run("touch " + path + " && echo 'Created: " + path + "'"); }
std::string create_dir(const std::string& path)    { return run("mkdir -p " + path + " && echo 'Created dir: " + path + "'"); }
std::string remove_file(const std::string& path)   { return run("rm -rf " + path + " && echo 'Removed: " + path + "'"); }
std::string copy(const std::string& src, const std::string& dst) { return run("cp -r " + src + " " + dst + " && echo 'Copied'"); }
std::string move(const std::string& src, const std::string& dst) { return run("mv " + src + " " + dst + " && echo 'Moved'"); }
std::string rename(const std::string& old_name, const std::string& new_name) { return move(old_name, new_name); }


std::string read(const std::string& path)    { return run("cat " + path + " 2>&1 | head -50"); }
std::string head(const std::string& path, int n) { return run("head -" + std::to_string(n) + " " + path + " 2>&1"); }
std::string tail(const std::string& path, int n) { return run("tail -" + std::to_string(n) + " " + path + " 2>&1"); }
std::string write(const std::string& path, const std::string& content) {
    return run("echo '" + content + "' > " + path + " && echo 'Written to: " + path + "'");
}
std::string append(const std::string& path, const std::string& content) {
    return run("echo '" + content + "' >> " + path + " && echo 'Appended to: " + path + "'");
}


std::string find(const std::string& path, const std::string& name)  { return run("find " + path + " -name '*" + name + "*' 2>/dev/null | head -20"); }
std::string grep(const std::string& pattern, const std::string& path) { return run("grep -rn '" + pattern + "' " + path + " 2>&1 | head -20"); }
std::string locate(const std::string& name)  { return run("locate " + name + " 2>&1 | head -20"); }


std::string chmod(const std::string& mode, const std::string& path) { return run("chmod " + mode + " " + path + " && echo 'Permissions set'"); }
std::string chown(const std::string& owner, const std::string& path) { return run("sudo chown " + owner + " " + path + " && echo 'Owner set'"); }
std::string file_info(const std::string& path) { return run("stat " + path + " 2>&1"); }


std::string compress(const std::string& path) { return run("tar -czf " + path + ".tar.gz " + path + " && echo 'Compressed: " + path + ".tar.gz'"); }
std::string extract(const std::string& archive) { return run("tar -xzf " + archive + " && echo 'Extracted: " + archive + "'"); }


std::string dispatch(const std::string& action, const std::string& args) {
    if (action == "ls") return list(args);
    if (action == "tree") return tree(args);
    if (action == "pwd") return pwd();
    if (action == "size") return size(args);
    if (action == "touch") return create_file(args);
    if (action == "mkdir") return create_dir(args);
    if (action == "rm") return remove_file(args);
    if (action == "cat") return read(args);
    if (action == "find") return find(".", args);
    if (action == "grep") return grep(args, ".");
    if (action == "compress") return compress(args);
    if (action == "extract") return extract(args);
    if (action == "info") return file_info(args);
    return "[Unknown files action: " + action + "]";
}

}

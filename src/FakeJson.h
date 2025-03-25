#pragma once
#include <any>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

class FakeJson;
template <typename T>
concept FakeJsonable = (!std::is_same_v<std::remove_cvref_t<T>, FakeJson>) && (!std::is_same_v<std::remove_cvref_t<T>, std::any>);

class FakeJson
{
private:
    std::any value;
    std::map<std::string, FakeJson> value_map;
    std::vector<FakeJson> value_vector;

public:
    FakeJson() = default;

    FakeJson(int v) { value = v; }

    FakeJson(double v) { value = v; }

    FakeJson(const std::string& v) { value = v; }

    FakeJson(const char* v) { value = std::string(v); }

    FakeJson(bool v) { value = v; }

    template <FakeJsonable T>
    FakeJson(const std::vector<T>& v)
    {
        for (auto& v1 : v)
        {
            pushBack(v1);
        }
    }

    template <FakeJsonable T>
    FakeJson(const std::map<std::string, T>& m)
    {
        for (auto& p : m)
        {
            (*this)[p.first] = p.second;
        }
    }

    template <FakeJsonable T>
    FakeJson(const std::pair<std::string, T>& p) { (*this)[p.first] = p.second; }

    template <FakeJsonable T>
    FakeJson(const std::pair<const char*, T>& p) { (*this)[p.first] = p.second; }

    std::type_info const& type() const { return value.type(); }

    template <typename T>
    bool isType() const { return value.type() == typeid(T); }

    bool isInt() const { return isType<int>(); }

    bool isDouble() const { return isType<double>(); }

    bool isString() const { return isType<std::string>(); }

    bool isBool() const { return isType<bool>(); }

    bool isNull() const { return !value.has_value(); }

    bool hasValue() const { return value.has_value(); }

    bool isMap() const { return !value_map.empty(); }

    bool isVector() const { return !value_vector.empty(); }

    bool empty() const { return value_map.empty() && value_vector.empty(); }

    template <FakeJsonable T>
    operator T() { return to<T>(); }

    template <FakeJsonable T>
    T to()
    {
        if (isType<T>())
        {
            return std::any_cast<T>(value);
        }
        if (!value.has_value())
        {
            value = T();
        }
        return T();
    }

    template <>
    const char* to()
    {
        if (isType<std::string>())
        {
            return std::any_cast<std::string&>(value).c_str();
        }
        return "";
    }

    int toInt() { return to<int>(); }

    double toDouble() { return to<double>(); }

    std::string toString() { return to<std::string>(); }

    bool toBool() { return to<bool>(); }

    FakeJson& operator[](int v) { return value_vector[v]; }

    FakeJson& operator[](const std::string& v) { return value_map[v]; }

    FakeJson& operator[](const char* v) { return value_map[std::string(v)]; }

    void pushBack(const FakeJson& v) { value_vector.push_back(v); }

    void erase(const std::string& v) { value_map.erase(v); }    //需由上一级删除，而不能自己删除，下同

    void erase(int v)
    {
        if (v >= 0 && v < value_vector.size()) { value_vector.erase(value_vector.begin() + v); }
    }

    void clear()
    {
        value.reset();
        value_map.clear();
        value_vector.clear();
    }

    bool exist(const std::string& v) const { return value_map.count(v); }

    bool exist(int v) const { return v >= 0 && v < value_vector.size(); }

    template <typename T>
    std::vector<T> toVector() const
    {
        std::vector<T> v;
        for (auto& i : value_vector)
        {
            v.emplace_back(i.to<T>());
        }
        return v;
    }

    template <typename T>
    std::map<std::string, T> toMap() const
    {
        std::map<std::string, T> v;
        for (auto& i : value_map)
        {
            v[i.first] = i.second.to<T>();
        }
        return v;
    }

    const std::map<std::string, FakeJson>& asMap() const { return value_map; }

    const std::vector<FakeJson>& asVector() const { return value_vector; }

    bool isPrintable() const
    {
        return isType<int>() || isType<double>() || isType<std::string>() || isType<bool>();
    }

    bool isNum() const { return isType<int>() || isType<double>(); }

    std::string to_string() const
    {
        if (isType<int>())
        {
            return std::to_string(std::any_cast<int>(value));
        }
        if (isType<double>())
        {
            return std::to_string(std::any_cast<double>(value));
        }
        if (isType<std::string>())
        {
            return std::any_cast<std::string>(value);
        }
        if (isType<bool>())
        {
            return std::any_cast<bool>(value) ? "true" : "false";
        }
        if (isNull())
        {
            return "null";
        }
        return "";
    }

    void parse(const std::string& str)
    {
        FakeJson& o = *this;
        std::vector<FakeJson*> ptr{ &o };
        int ignore_space = 1;
        char quote = '\0';
        std::string cur;

        auto try_to_any = [](std::string&& str1) -> std::any
        {
            auto str = std::move(str1);
            str1.clear();
            if (str.empty())
            {
                return std::any{};
            }
            if (str[0] == '\"' && str.back() == '\"')
            {
                return std::any{ str.substr(1, str.size() - 2) };
            }
            if (str[0] == '\'' && str.back() == '\'')
            {
                return std::any{ str.substr(1, str.size() - 2) };
            }
            if (str == "true")
            {
                return std::any{ true };
            }
            if (str == "false")
            {
                return std::any{ false };
            }
            if (str == "null")
            {
                return std::any{};
            }
            char* end = nullptr;
            auto i = strtoll(str.c_str(), &end, 10);
            if (end == str.c_str() + str.size())
            {
                return std::any{ int(i) };
            }
            char* end2 = nullptr;
            auto d = strtod(str.c_str(), &end2);
            if (end2 == str.c_str() + str.size())
            {
                return std::any{ d };
            }
            return std::any{ std::move(str) };
        };

        auto dequote = [](std::string& str) -> std::string
        {
            if (str.empty())
            {
                return str;
            }
            if (str[0] == '\"' && str.back() == '\"')
            {
                return str.substr(1, str.size() - 2);
            }
            if (str[0] == '\'' && str.back() == '\'')
            {
                return str.substr(1, str.size() - 2);
            }
            return str;
        };

        for (auto& c : str)
        {
            if (c == '\"' || c == '\'')
            {
                if (quote == '\0')
                {
                    quote = c;
                    //continue;
                }
                else if (quote == c)
                {
                    quote = '\0';
                    //continue;
                }
            }
            if (quote == '\0')
            {
                if (c == '[' || c == '{')
                {
                    ptr.back()->value = try_to_any(std::move(cur));
                    ptr.back()->value_vector.emplace_back();    //create a new one
                    ptr.push_back(&ptr.back()->value_vector.back());
                    ignore_space = 1;
                }
                else if (c == ']' || c == '}')
                {
                    ptr.back()->value = try_to_any(std::move(cur));
                    if (!ptr.back()->value.has_value() && ptr.back()->empty()
                        && !ptr[ptr.size() - 2]->value_vector.empty() && &ptr[ptr.size() - 2]->value_vector.back() == ptr.back())
                    {
                        ptr[ptr.size() - 2]->value_vector.pop_back();
                    }
                    if (ptr.size() >= 2)
                    {
                        ptr.pop_back();
                    }
                }
                else if (c == ':')
                {
                    ptr.back() = &ptr[ptr.size() - 2]->value_map[dequote(cur)];
                    cur.clear();
                    ptr[ptr.size() - 2]->value_vector.pop_back();
                    ignore_space = 1;
                }
                else if (c == ',')
                {
                    ptr.back()->value = try_to_any(std::move(cur));
                    if (ptr.size() >= 2)
                    {
                        ptr[ptr.size() - 2]->value_vector.emplace_back();    //change to a new one
                        ptr.back() = &ptr[ptr.size() - 2]->value_vector.back();
                    }
                    ignore_space = 1;
                }
                else if ((c == ' ' || c == '\n' || c == '\r') && ignore_space == 1)
                {
                }
                else
                {
                    cur += c;
                    ignore_space = 1;
                }
            }
            else
            {
                cur += c;
                ignore_space = 0;
            }
        }
        //return o;
    }

    std::string allToString(bool narrow = true, int space = 0) const
    {
        if (narrow)
        {
            space = 0;
        }
        std::string res;
        if (hasValue())
        {
            //res += std::string(space, ' ');
            if (isPrintable())
            {
                if (isNum())
                {
                    res += to_string();
                }
                else if (isType<bool>())
                {
                    res += to_string();
                }
                else
                {
                    res += "\"" + to_string() + "\"";
                }
            }
            else
            {
                res += "\"Unknown type: ";
                res += value.type().name();
                res += "\"";
            }
            /*if (!narrow)
            {
                res += '\n';
            }*/
        }
        else
        {
            if (empty())
            {
                res += "null";
            }
            if (!value_map.empty())
            {
                if (!narrow)
                {
                    res += '\n';
                }
                res += std::string(space, ' ');
                res += '{';
                if (!narrow)
                {
                    res += '\n';
                }
                for (auto& i : value_map)
                {
                    auto valstr = i.second.allToString(narrow, space + 4);
                    if (!narrow)
                    {
                        res += std::string(space + 4, ' ');
                    }
                    res += "\"" + i.first + "\": " + valstr + ",";
                    if (narrow)
                    {
                        res += ' ';
                    }
                    else
                    {
                        res += '\n';
                    }
                }
                res.pop_back();
                if (narrow)
                {
                    res.back() = '}';
                }
                else
                {
                    res.pop_back();
                    res += '\n' + std::string(space, ' ') + '}';
                }
            }
            if (!value_vector.empty())
            {
                if (!narrow)
                {
                    res += '\n';
                }
                res += std::string(space, ' ');
                res += '[';
                if (!narrow)
                {
                    res += '\n';
                }
                for (auto& i : value_vector)
                {
                    if (!narrow)
                    {
                        res += std::string(space + 4, ' ');
                    }
                    res += i.allToString(narrow, space + 4) + ",";
                    if (narrow)
                    {
                        res += ' ';
                    }
                    else
                    {
                        res += '\n';
                    }
                }
                res.pop_back();
                if (narrow)
                {
                    res.back() = ']';
                }
                else
                {
                    res.pop_back();
                    res += '\n' + std::string(space, ' ') + ']';
                }
            }
        }
        return res;
    }
};

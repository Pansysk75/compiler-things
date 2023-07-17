/*
Author: Panos Syskakis, July 2023


 */

#include <string>
#include <vector>
#include <iostream>

// Calculate the failure function values, to use in KMP algorithm
std::vector<int> calculate_failure_function(const std::string &str)
{
    std::vector<int> f(str.size(), 0);
    int x = 0;
    for (int y = 1; y < str.size(); y++)
    {
        // Find longest prefix that is also a postfix
        while (x > 0 && str[y] != str[x])
        {
            x = f[x - 1];
        }
        if (str[y] == str[x])
        {
            x = x + 1;
            f[y] = x;
        }
        else
        {
            f[y] = 0;
        }
    }
    return f;
}

// KMP algorithm, finds the first occurance of string b in string a, using
// a vector of failure function values (f)
// On match, returns an integer indicating "how far in" string a we found string b
// Returns -1 on no match.
int match_kmp(std::string &a, std::string &b, std::vector<int> &f)
{
    int s = 0;
    for (int i = 0; i < a.size(); i++)
    {
        while (s > 0 && a[i] != b[s])
            s = f[s - 1];
        if (a[i] == b[s])
            s = s + 1;
        if (s == b.size())
            return (i - s + 1);
    }
    return -1;
}

int main()
{
    std::string a = "wacabaab";
    std::string b = "abaa";
    std::cout << "Text: " << a << std::endl;
    std::cout << "String: " << b << std::endl;

    std::vector<int> f = calculate_failure_function(b);

    // Print result
    std::cout << "Failure function values for string: " << std::endl;
    for (auto &x : f)
    {
        std::cout << x << " ";
    }

    std::cout << std::endl;

    std::cout << "KMP algorithm returned: " << match_kmp(a, b, f);
}
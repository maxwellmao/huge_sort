/*
 * =====================================================================================
 *
 *       Filename:  huge_sort.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/18/2016 01:51:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Maxwell Weixuan Mao (Maxwell), maxwell.mao@hotmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>

// djb2_hash
// http://www.cse.yorku.ca/~oz/hash.html
unsigned long
hash(const char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

bool StrLess(const std::string &a, const std::string &b)
{
    // can be customized to other comparer for different tasks
    return a<b;
}

typedef bool (*Comparer)(const std::string &, const std::string & );

class PartNode
{
public:
    int part;
    std::string content;
};

class HugeSort{
private:
    void sortSingleParts(Comparer comp);

    void mergeSort(const char *save_path);

    std::map<unsigned int, std::ofstream *> part_file_descriptors;
    
    std::map<unsigned int, std::ifstream *> sorted_part_file_descriptors;

    // file path of each small part
    std::vector<std::string> part_file_path;

    // temporay directory to save middle results
    std::string tmp_dir;

    unsigned int total_part;

    void closePartFiles()
    {
        for(std::map<unsigned int, std::ofstream *>::iterator it=this->part_file_descriptors.begin(); it!=this->part_file_descriptors.end(); it++)
        {
            if(*(it->second))
            {
                it->second->close();
            }
            delete it->second;
        }
        this->part_file_descriptors.clear();

        for(std::map<unsigned int, std::ifstream *>::iterator it=this->sorted_part_file_descriptors.begin(); it!=this->sorted_part_file_descriptors.end(); it++)
        {
            if(*(it->second))
            {
                it->second->close();
            }
            delete it->second;
        }
        this->sorted_part_file_descriptors.clear();
    }

public:
    HugeSort(const std::string &dir, const unsigned int &total=1000): tmp_dir(dir), total_part(total)
    {
        this->part_file_path.assign(total, "");
    }

    ~HugeSort()
    {
        this->closePartFiles();
    }

    void start(const char* load_path, const char *save_path, Comparer comp=StrLess);
    
    void start(std::istream &in_stream, const char *save_path, Comparer comp=StrLess);
};

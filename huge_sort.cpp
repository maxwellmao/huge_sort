/*
 * =====================================================================================
 *
 *       Filename:  huge_sort.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/18/2016 01:52:05 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Maxwell Weixuan Mao (Maxwell), maxwell.mao@hotmail.com
 *        Company:  
 *
 * =====================================================================================
 */

#include <queue>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <assert.h>
#include "huge_sort.h"

using namespace std;

void HugeSort::sortSingleParts(Comparer comp)
{
    // use built-in sorting function to sort each part
    for(unsigned int index=0; index < this->part_file_path.size(); index++)
    {
        ifstream fp(this->part_file_path[index].c_str());

        if(!fp)
        {
            cerr << "Error in sorting part file " << this->part_file_path[index] << endl;
            return;
        }
        vector<string> content;

        string line;

        while(fp >> line)
        {
            content.push_back(line);
        }
        sort(content.begin(), content.end(), comp);

        ofstream wp((this->part_file_path[index]+".sort").c_str());

        if(!wp)
        {
            cerr << "Error in writing sorted part file into " << this->part_file_path[index]+".sort" << endl;
            
            fp.close();

            return;
        }

        for(unsigned int line_index=0; line_index<content.size(); line_index++)
        {
            wp << content[line_index] << endl;
        }

        wp.close();

        fp.close();
    }
}

class PartNodeCompare
{
    public:
        bool operator() (const PartNode &a, const PartNode &b)
        {
            return !StrLess(a.content, b.content);
        }
};

void HugeSort::mergeSort(const char *save_path)
{
    ofstream wp(save_path);

    priority_queue<PartNode, std::vector<PartNode>,  PartNodeCompare> node_queue;

    for(unsigned int index=0; index < this->part_file_path.size(); index++)
    {
        ifstream *fp=new ifstream((this->part_file_path[index]+".sort").c_str());

        PartNode part_node;

        part_node.part=index;

        (*fp) >> part_node.content;

        node_queue.push(part_node);

        pair<map<unsigned int, ifstream *>::iterator, bool> ret=this->sorted_part_file_descriptors.insert(make_pair(index, fp));

        assert(ret.second);
    }

    while(this->sorted_part_file_descriptors.size()>0 && !node_queue.empty())
    {
        PartNode part_node=node_queue.top();
        
        node_queue.pop();

        part_node.part=part_node.part;

//        cout << part_node.part << part_node.content << endl;

        wp << part_node.content << endl;

        map<unsigned int, ifstream *>::iterator it=this->sorted_part_file_descriptors.find(part_node.part);

        if(it!=this->sorted_part_file_descriptors.end())
        {
            if((*it->second) >> part_node.content)
            {
                node_queue.push(part_node);
            }
            else
            {
                it->second->close();
                delete it->second;

                this->sorted_part_file_descriptors.erase(part_node.part);
            }
        }
    }
    while(!node_queue.empty())
    {
        PartNode part_node=node_queue.top();

        wp << part_node.content << endl;

        node_queue.pop();
    }

    wp.close();
}

void HugeSort::start(const char* load_path, const char *save_path, Comparer comp)
{
    ifstream fp(load_path);

    if(!fp)
    {
        cerr << "Cannot load file from " << load_path << endl;
        return;
    }

    string line;

    while(fp >> line)
    {
        unsigned int part=hash(line.c_str()) % this->total_part;

        char part_buf[8];

        sprintf(part_buf, "%06d", part);

        string part_str(part_buf);

        map<unsigned int, ofstream *>::iterator it=this->part_file_descriptors.find(part);

        if(it==this->part_file_descriptors.end())
        {
            string path=this->tmp_dir+"/part-"+part_str;

            this->part_file_path[part]=path;

            ofstream *wp=new ofstream(path.c_str());

            if(!(*wp))
            {
                cerr << "Cannot write into " << path << endl;

                fp.close();

                return;
            }

            pair<map<unsigned int, ofstream *>::iterator, bool> ret=this->part_file_descriptors.insert(make_pair(part, wp));

            assert(ret.second);

            it=ret.first;
        }
        *(it->second) << line << endl;
    }

    fp.close();

    this->closePartFiles();

    this->sortSingleParts(comp);

    this->mergeSort(save_path);
}

string tmp_dir;
string load_path;
unsigned int parts=1000;
string save_path;

void usage(char *prog_name, const char *more){
    cerr << more;
    cerr << "Usage: " << prog_name << " [-T] [-p] [-l] [-s]" << endl;
    cerr << "-T temporay directory for storing middle results" << endl;
    cerr << "-p total number of splits for sorting" << endl;
    cerr << "-l path of the huge file" << endl;
    cerr << "-s path of the output" << endl;
    exit(0);
}

void parse_args(int argc, char *argv[])
{
    for (int i = 1; i < argc; i++){
        if(argv[i][0] == '-') {
            switch(argv[i][1]){
                case 'T':
                    tmp_dir=argv[i+1];
                    i++;
                    break;

                case 'p':
                    parts=atoi(argv[i+1]);
                    i++;
                    break;

                case 'l':
                    load_path=argv[i+1];
                    i++;
                    break;

                case 's':
                    save_path=argv[i+1];
                    i++;
                    break;

                case 'h':
                    usage(argv[0], "");
                    break;

                default:
                    usage(argv[0], "Unknown parameters\n");
                    break;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    parse_args(argc, argv);
    HugeSort *huge_sort=new HugeSort(tmp_dir, parts);
    huge_sort->start(load_path.c_str(), save_path.c_str());

    return 0;
}

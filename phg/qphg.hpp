/***************************************************************************
                        QT PHG
            用于方便控制电脑以及工作相关的各类事务.
/**************************************************************************/
#pragma once
#include <vector>
#include <string>
#include <sstream>
#include <filesystem>
#include <map>
#include <QMessageBox>
#include <QDir>

#define real        float
#define PRINT(msg)  {std::stringstream __ss; __ss << msg; printf(__ss.str().c_str());printf("\n");}
#define PRINTV(v) PRINT(#v << " = " << v)

#define ELEMENT		int
#define var			ELEMENT
#define PHG_ARGS    (cd.valstack.top + 1)
#define PHG_PARAM(index)	cd.valstack.get(args - index)
#define PHG_STABLE(index)	gstable[index - 1]
#define PHGAPI(name)    static var name(code& cd, int args)
#define REGAPI(name)  register_api(#name, phg::name)

namespace phg
{
    inline void _PHGPRINT(const std::string& pre, var v)
    {
        printf(pre.c_str());
    }

    // ------------------------------------------
    #include "phg.hpp"
    // ------------------------------------------
    inline const char* chars2name(code& cd)
        {
            static char buff[128];
            int i = 0;
            for (; i < 128; i++) {
                char c = cd.cur();
                if (!isdigit(c) && !isname(c))
                    break;

                buff[i] = c;
                if (c != ']')
                    cd.next();
            }
            buff[i] = '\0';

            return buff;
        }

        inline real chars2real(code& cd)
        {
            static char buff[128];
            int i = 0;
            for (; i < 128; i++) {
                char c = cd.cur();
                if (!isdigit(c) && c != '.')
                    break;

                buff[i] = c;
                if (c != ']')
                    cd.next();
            }
            buff[i] = '\0';

            return atof(buff);
        }

        // table
        static void _table(code& cd)
        {
            PRINT("TABLE:");
            cd.next();

            std::string str;

            while (!cd.eoc()) {
                char c = cd.cur();
               // PRINT("c=" << int(c));
                if (c == ']') {
                    cd.next();
                   // cd.next();
                    break;
                }
                if (c == ';') {
                    cd.next();
                    break;
                }
                else {
                    while (cd.cur() != ']')
                    {
                        str += cd.cur();
                        cd.next();
                    }
                    //str += '\0';
                    PRINT(str.c_str());
                    gstable.push_back(str);
                    str = "";
                }
            }
        }
        /* tree
            {
                a: 1;
                b: 2;
                {
                    a: 3;
                    b: 4;
                }
            }
        */
        struct tree_t
        {
            std::map<std::string, std::string> kv;
            std::vector<tree_t*> children;

            static void clear(tree_t* ot)
            {
                for (auto it : ot->children)
                {
                     clear(it);
                }
                delete(ot);
            }
        };

        static void _tree(code& cd, tree_t* tree, int depth = 0)
        {
            PRINT("TREE(" << depth << "):");
            cd.next();

            std::string key, val;
            std::string* pstr = &key;
            while (!cd.eoc()) {
                char c = cd.cur();
                //PRINT("c=" << c );
                if (c == '{') {
                    tree_t* ntree = new tree_t;
                    tree->children.push_back(ntree);
                    _tree(cd, ntree, depth + 1);
                }
                else if (c == '}') {
                    PRINT("}")
                    return;
                }
                else if (c == ';') {
                    tree->kv[key] = val;
                    PRINT(key << ":" << val);
                    key = "";
                    val = "";
                    pstr = &key;

                    cd.next();
                }
                else if (c == '\n') {
                    cd.next();
                }
                else if (c == ':') {

                    pstr = &val;
                    cd.next();
                }
                else {
                    *pstr += cd.cur();
                    cd.next();
                }
            }
        }
        static void _tree(code& cd)
        {
            tree_t* tree = new tree_t;
            _tree(cd, tree, 0);

            tree_t::clear(tree);
        }
    // ------------------------------------------
    inline bool loadtemp(const std::string& name, std::string & contents)
    {
        FILE* file = NULL;
        std::string fullpath = name;
        fopen_s(&file, fullpath.c_str(), "r");
        if (!file)
        {
            PRINT("loadtemp failed : " << name.c_str());
            return false;
        }

        char strbuf[256];
        while (fgets(strbuf, 256, file))
        {
            contents += strbuf;
        }
        fclose(file);
        return true;
    }
    bool writecontent(const std::string& name, std::string& contents)
    {
        FILE* file = NULL;
        std::string fullpath = name;
        fopen_s(&file, fullpath.c_str(), "w");
        if (!file)
        {
            PRINT("writecontent failed : " << name.c_str());
            return false;
        }
        fwrite(contents.c_str(), contents.length(), 1, file);
        fclose(file);
        return true;
    }

    // ******************************************
    // API
    // ******************************************

    PHGAPI(msgbox)
    {
        QMessageBox::information(NULL, "", "OK!");
        return 0;
    }
  
    PHGAPI(paint)
     {
         system("mspaint");
         return 0;
     }
    PHGAPI(vcpkg)
     {
        QString curpath = QDir::currentPath();
        QDir::setCurrent("C:\\Users\\18858\\Documents\\Work\\vcpkg-export-20210706-101500");
        system("vcpkg integrate install");
        system("pause");
        QDir::setCurrent(curpath);
         return 0;
     }
    // -----------------------------------
    // setup
    // -----------------------------------

    void setup()
    {
        PRINT("setup QPHG");
        phg::act = phg::act_default;
        phg::table = _table;
        phg::tree = _tree;

        REGAPI(msgbox);
        REGAPI(paint);
        REGAPI(qt);
        REGAPI(vcpkg);
    }
}

// ====================================
// test
// ====================================
void dophg()
{
    phg::dofile("code.txt");
}

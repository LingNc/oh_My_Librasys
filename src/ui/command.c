#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ui/command.h"
#include "Tools/Trie.h"
#include "ui/components/func.h"
#include "Tools/Vector.h"

#define NUM_COMMANDS 4

const char *commands[NUM_COMMANDS]={
    "set",
    "help",
    "search",
    "page"
};

static trie root;

// 初始化自动补全
void init_autocomplete(){
    root=create_node();
    build_trie(root,commands,NUM_COMMANDS);
}

vector find_words(trie node,char *prefix,int length){
    vector matches=new_vector("String");
    if(node->is_end_of_word){
        string match=new_string();
        match->assign_cstr(match,prefix);
        matches->push_back(matches,match);
    }
    for(int i=0; i<128; i++){
        if(node->children[i]){
            prefix[length]=i;
            prefix[length+1]='\0';
            vector child_matches=find_words(node->children[i],prefix,length+1);
            for(size_t j=0; j<child_matches->size(child_matches); j++){
                matches->push_back(matches,child_matches->at(child_matches,j));
            }
            child_matches->free(child_matches);
        }
    }
    return matches;
}

void autocomplete(trie root,char *input,bool *show_matches,int *index,char *prefix){
    trie node=root;
    int len=strlen(prefix);
    for(int i=0;i<len;i++){
        int idx=(unsigned char)prefix[i];
        if(!node->children[idx]){
            return;
        }
        node=node->children[idx];
    }

    // 刷新补全到分支的内容
    refresh(input-1,index,input-1);
    // 如果无须补全直接返回
    if(node->is_end_of_word)
        return;
    char last_char;
    int children_count=count_children(node,&last_char);

    if(children_count==1){
        input[(*index)-1]=last_char;
        input[*index]='\0';
        *index+=1;
        autocomplete(node,input,show_matches,index,prefix+len);
    }
    else{
        if(*show_matches){
            // 忽略第一个字符 '/'
            vector matches=find_words(node,input,strlen(input));
            char oldContent[MAX_COMMAND_LENGTH]={0};
            strcpy(oldContent,input-1);
            if(matches->size(matches)>0){
                char newContent[MAX_COMMAND_LENGTH]={ 0 };
                strcpy(newContent,input-1);
                strcat(newContent,"\n");
                for(size_t i=0; i<matches->size(matches); i++){
                    string match=(string)matches->at(matches,i);

                    strcat(newContent,match->c_str(match));
                    strcat(newContent," ");
                }
                refresh(input-1,index,newContent);
                char temp;
                // 按任意键返回
                getch();
                refresh(input-1,index,oldContent);
            }
            matches->free(matches);
            *show_matches=false;
        }
        else{
            *show_matches=true;
        }
    }
}

void execute(void **args){
    int *page_size=(int *)args[0];
    int *now_page = (int *)args[2];
    int *total_pages = (int *)args[3];

    char input[MAX_COMMAND_LENGTH]={ 0 };
    int index=0;
    char ch;
    bool show_matches=false;

    input[index++]='/';
    printf("%s",input);
    while((ch=getch())!='\n'){
        if(ch=='\t'){
            input[index]='\0';
            autocomplete(root,input+1,&show_matches,&index,input+1);
        }
        else if(ch==127){ // 处理退格键
            backspace(input,&index);
            if(index==0){
                printf("\n退出命令模式\n");
                return;
            }
        }
        else{
            if(index<MAX_COMMAND_LENGTH-1){
                input[index++]=ch;
                putchar(ch);
            }
            show_matches=false;
        }
    }
    input[index]='\0';

    if(search(root,input+1)){ // 忽略第一个字符 '/'
        if(strncmp(input+1,"set",3)==0){
            int new_size=atoi(input+5);
            if(new_size>0){
                *page_size=new_size;
                *now_page=0;
                *total_pages=(*total_pages+*page_size-1)/(*page_size);
                printf("\n每页显示数量已设置为 %d\n",*page_size);
            }
            else{
                printf("\n无效参数 [nums]\n");
            }
        }
        else if(strcmp(input+1,"help")==0){
            printf("\n输入q退出\n");
            printf("/set [nums] - 调整每一页显示的数量\n");
            printf("/search [keyword] - 搜索关键字\n");
            printf("/page [nums] - 跳转到指定页码\n");
        }
        else if(strncmp(input+1,"search",6)==0){
            const char *keyword = input+8;
            printf("\n搜索关键字: %s\n", keyword);
            // 调用数据库搜索函数
            // 示例: vector results = db->search(db, keyword);
            // 显示搜索结果
        }
        else if (strncmp(input + 1, "page", 4) == 0) {
            int target_page = atoi(input + 6) - 1;
            if (target_page >= 0 && target_page < *total_pages) {
                *now_page = target_page;
                printf("\n跳转到第 %d 页\n", target_page + 1);
            } else {
                printf("\n无效页码 [nums]\n");
            }
        }
    }
    else{
        printf("\n无效命令，输入 /help 查看帮助\n");
    }
    getch();
}
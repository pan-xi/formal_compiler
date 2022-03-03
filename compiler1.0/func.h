#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string>
#include<cstring>
#include<stdlib.h>
#include<vector>
#include <iostream>
#include<Windows.h>
#include<setjmp.h>
#include<stack>

#define Token int
#define MAXSIZE 1000
#define MAX 200
#define isunderline(c) c=='_'
#define return_error(type) do{Row_Error = pword->row;longjmp(jmpbuf, type); }while(0)//后边经常用，包成宏函数得了，不值当包成函数
#define istype (pword->token <= TK_KW_VOID && pword->token >= TK_KW_INT)
using namespace std;

typedef struct {
	Token token;
	string s;
	int row;
}TkWord,*pTkWord;

enum TokenCode
{
	//运算符和分隔符和EOF
	TK_START_L,//(
	TK_END_L,//)
	TK_START_M,//[
	TK_END_M,//]
	TK_BEGIN,//{
	TK_END,//}
	TK_MEMBER,//->
	TK_ADD_SELF,//++
	TK_DE_SELF,//--
	
	TK_NON,//!
	TK_SOF,//SIZEOF
	TK_PLUS,//+
	TK_REDUCE,//-
	TK_STAR,//*
	TK_DIVIDE,// /
	TK_MOD,// %
	TK_LEFT,//<<
	TK_RIGHT,//>>
	TK_LARGE,//>
	TK_LITTLE,//<
	TK_BQ,//>=
	TK_LQ,//<=
	TK_EQUL,//==
	TK_NEQ,//!=
	TK_AND,//&仅作为和运算
	TK_XOR,//^
	TK_OR,//|
	TK_LAND,//&&  logic and
	TK_LOR,//||
	//TK_CHOICE,//?:先搁置吧
	TK_GIVEVAL,//=
	TK_D_GV,// /=
	TK_M_GV,// *=
	TK_A_GV,//+= ADD GIVE VAL
	TK_RE_GV,//-=
	TK_L_GV,//<<=
	TK_R_GV,// >>=
	TK_AND_GV,//&=
	TK_X_GV,//^=
	TK_O_GV,//|=
	TK_SEMICOLON,//;
	TK_COMMA,//,
	TK_EOF,//EOF

	//常量
	TK_CNUM,//CONST NUM
	//TK_CFLOAT,float,SHORT,DOUBLE等跟int归一起好了
	TK_CSTRING,
	TK_CCHAR,

	//关键字
	TK_KW_INT,//KEY WORDS INT
	TK_KW_SHORT,
	TK_KW_CHAR,
	TK_KW_DOUBLE,
	TK_KW_FLOAT,
	TK_KW_LONG,
	TK_KW_TYPEDEF,
	TK_KW_LONGLONG,
	TK_KW_CONST,
	TK_KW_VOID,
	TK_KW_STRUCT,
	TK_KW_FOR,
	TK_KW_IF,
	TK_KW_WHILE,
	TK_KW_ELSE,
	//TK_KW_TYPEDEF,//typedef拿到前面去了
	TK_KW_DEFINE,
	TK_KW_INCLUDE,
	TK_KW_BREAK,
	TK_KW_RETURN,
	TK_KW_CONTINU,
	TK_KW_ENUM,

	//注释
	TK_MEANING,//  //

	//函数名和变量名
	TK_FUNCT,//函数名
	TK_VAL,//变量名
	TK_DEFINE,//DEFINE
	TK_INCLUDE,//INCLUDE
}; 

enum Error_type {
	PROGRAM_BEGIN,//执行语法分析,默认状态为0则进入语法分析
	SUCC,//成功
	SOMETHINGWRONG,	//未知错误
	LACK_SEMICOLON,	//缺少分号
	RETURN_ERROR,//return类型错误
	BREAK_LACK_SEMICOLON,//break缺少分号
	CONTINU_LACK_SEMICOLON,	//continue缺少分号
	EXTER_DEC,//外部声明处出现错误
	LACK_TYPE,//缺少类型
	VAL_WRONG,//变量错误
	FUNC_WRONG,//函数错误
	LACK_IDENT,//缺少标识符
	LACK_START_L ,//缺少左圆括号
	LACK_END_L,//缺少右圆括号
	LACK_START_M,//缺少左中括号
	LACK_END_M,//缺少右中括号
	LACK_BEGIN,//缺少左花括号
	LACK_END,//缺少右花括号
	NESTED_FUNC_DECLEAR,//函数嵌套定义
	IF_LACK_START_L,//if缺少左圆括号
	IF_LACK_END_L,	//if缺少右圆括号
	FOR_LACK_START_L,	//for缺少左圆括号
	FOR_LACK_END_L,	//for缺少右圆括号
	PRIMARY_ERROR,//初等表达式错误
	FUNCVAR_WRONG,//函数调用变量错误
	LACK_ST,//缺少语句
	CONTINU_ERROR,//continue使用错误
	ELSE_ERROR,//else使用错误
	FOR_ERROR,//for使用错误
	WHILE_LACK_START_L,	//while缺少左圆括号
	WHILE_LACK_END_L,	//while缺少右圆括号
	WHILE_ERROR,//while使用错误
	IDENTIFY_ERROR,//标识符错误，不能以数字开头
	VAL_EXCESSIVE,//变量过多
	VAL_ERROR,//数据错误
};

void word_analysis(FILE* fp);//进入词法分析
void hash_table(vector <TkWord>* hashtable, int keywordslen);//处理哈希
void meaning_p( int* p, char* tmp_c);//处理注释
void keyword_p( int* p, char* tmp_c, vector <TkWord>* hashtable);//处理关键字
void num_p(int* p, char* tmp_c, vector <TkWord>* hashtable);//处理数字
void char_p(int* p, char* tmp_c);//处理char
void str_p(int* p, char* tmp_c);//处理str
void else_p(int* p, char* tmp_c, vector <TkWord>* hashtable);//处理剩下的符号
void next_target(string* tmp_s, char* tmp_c, int* p);//下一个目标
void printbegin();
void Print(string , int );//打印颜色
void PrintWhite();//句柄再刷白
void Compile();//语法分析入口
void External_Claim();//外部声明
void type_judge();//判断外部声明是变量还是函数
void val_process(int);//处理声明变量
void func_process();//处理声明函数
void func_val_process();//处理形参
void func_body_process();//处理函数体
void typedef_struct();//处理typedef
void typedef_name();//处理重命名
void func_char();//处理char
void func_num();//处理num
void for_process();
void if_process();
void while_process();
void else_process();
void return_process();
void enum_process();
void func_func();//分析函数调用
void func_val();//分析函数调用里的参数
void primary_analysis();//分析表达式入口，以下函数均为分析表达式的函数
void C();
void next();
void S();
void L();
void J();
void M();
void P();
void H();
void T();
void U();

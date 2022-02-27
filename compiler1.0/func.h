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
#define return_error(type) do{Row_Error = pword->row;longjmp(jmpbuf, type); }while(0)//��߾����ã����ɺ꺯�����ˣ���ֵ�����ɺ���
#define istype (pword->token <= TK_KW_VOID && pword->token >= TK_KW_INT)
using namespace std;

typedef struct {
	Token token;
	string s;
	int row;
}TkWord,*pTkWord;

enum TokenCode
{
	//������ͷָ�����EOF
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
	TK_AND,//&����Ϊ������
	TK_XOR,//^
	TK_OR,//|
	TK_LAND,//&&  logic and
	TK_LOR,//||
	//TK_CHOICE,//?:�ȸ��ð�
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

	//����
	TK_CNUM,//CONST NUM
	//TK_CFLOAT,float,SHORT,DOUBLE�ȸ�int��һ�����
	TK_CSTRING,
	TK_CCHAR,

	//�ؼ���
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
	//TK_KW_TYPEDEF,//typedef�õ�ǰ��ȥ��
	TK_KW_DEFINE,
	TK_KW_INCLUDE,
	TK_KW_BREAK,
	TK_KW_RETURN,
	TK_KW_CONTINU,
	TK_KW_ENUM,

	//ע��
	TK_MEANING,//  //

	//�������ͱ�����
	TK_FUNCT,//������
	TK_VAL,//������
	TK_DEFINE,//DEFINE
	TK_INCLUDE,//INCLUDE
}; 

enum Error_type {
	PROGRAM_BEGIN,//ִ���﷨����,Ĭ��״̬Ϊ0������﷨����
	SUCC,//�ɹ�
	SOMETHINGWRONG,	//δ֪����
	LACK_SEMICOLON,	//ȱ�ٷֺ�
	RETURN_ERROR,//return���ʹ���
	BREAK_LACK_SEMICOLON,//breakȱ�ٷֺ�
	CONTINU_LACK_SEMICOLON,	//continueȱ�ٷֺ�
	EXTER_DEC,//�ⲿ���������ִ���
	LACK_TYPE,//ȱ������
	VAL_WRONG,//��������
	FUNC_WRONG,//��������
	LACK_IDENT,//ȱ�ٱ�ʶ��
	LACK_START_L ,//ȱ����Բ����
	LACK_END_L,//ȱ����Բ����
	LACK_START_M,//ȱ����������
	LACK_END_M,//ȱ����������
	LACK_BEGIN,//ȱ��������
	LACK_END,//ȱ���һ�����
	NESTED_FUNC_DECLEAR,//����Ƕ�׶���
	IF_LACK_START_L,//ifȱ����Բ����
	IF_LACK_END_L,	//ifȱ����Բ����
	FOR_LACK_START_L,	//forȱ����Բ����
	FOR_LACK_END_L,	//forȱ����Բ����
	PRIMARY_ERROR,//���ȱ��ʽ����
	FUNCVAR_WRONG,//�������ñ�������
	LACK_ST,//ȱ�����
	CONTINU_ERROR,//continueʹ�ô���
	ELSE_ERROR,//elseʹ�ô���
	FOR_ERROR,//forʹ�ô���
	WHILE_LACK_START_L,	//whileȱ����Բ����
	WHILE_LACK_END_L,	//whileȱ����Բ����
	WHILE_ERROR,//whileʹ�ô���
	IDENTIFY_ERROR,//��ʶ�����󣬲��������ֿ�ͷ
	VAL_EXCESSIVE,//��������
	VAL_ERROR,//���ݴ���
};

void word_analysis(FILE* fp);//����ʷ�����
void hash_table(vector <TkWord>* hashtable, int keywordslen);//�����ϣ
void meaning_p( int* p, char* tmp_c);//����ע��
void keyword_p( int* p, char* tmp_c, vector <TkWord>* hashtable);//����ؼ���
void num_p(int* p, char* tmp_c, vector <TkWord>* hashtable);//��������
void char_p(int* p, char* tmp_c);//����char
void str_p(int* p, char* tmp_c);//����str
void else_p(int* p, char* tmp_c, vector <TkWord>* hashtable);//����ʣ�µķ���
void next_target(string* tmp_s, char* tmp_c, int* p);//��һ��Ŀ��
void printbegin();
void Print(string , int );//��ӡ��ɫ
void PrintWhite();//�����ˢ��
void Compile();//�﷨�������
void External_Claim();//�ⲿ����
void type_judge();//�ж��ⲿ�����Ǳ������Ǻ���
void val_process(int);//������������
void func_process();//������������
void func_val_process();//�����β�
void func_body_process();//��������
void typedef_struct();//����typedef
void typedef_name();//����������
void func_char();//����char
void func_num();//����num
void for_process();
void if_process();
void while_process();
void else_process();
void return_process();
void enum_process();
void func_func();//������������
void func_val();//��������������Ĳ���
void primary_analysis();//�������ʽ
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
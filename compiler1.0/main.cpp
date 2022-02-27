#include"func.h"
//#define MAXSZIE 1000
TkWord word[MAXSIZE] = { 0 };//用来存分好的词，func.cpp也要用，所以设了全局变量
int wordlen;//存入多少结构体了
int Row_Error;//存错误行数
jmp_buf jmpbuf;//断点
int JmpJud;//是否编译成功的标志



int main(int argc,char*argv[]) {
	if (argc != 2) {
		printf("请传入一个文件");
			return -1;
	}
	FILE* fp = fopen(argv[1], "r");//传入要读入的文件，健壮性强一点
	if (NULL == fp) {
		perror("fopen");
	}
	word_analysis(fp);//进入词法分析

	/*此处词法分析已结束
	下面进入语法分析*/
	
	JmpJud = setjmp(jmpbuf);
	string res;//存要打印的字符
	

	switch (JmpJud)
	{
	case SOMETHINGWRONG:res = "编译失败，未知错误！"; break;
	case LACK_SEMICOLON:res = "编译失败，缺少';'！"; break;
	case EXTER_DEC:res = "编译失败，外部变量或函数错误！"; break;
	case LACK_TYPE:res = "编译失败，缺少类型符！"; break;
	case VAL_WRONG:res = "编译失败，变量错误！"; break;
	case FUNC_WRONG:res = "编译失败，函数错误！"; break;
	case LACK_IDENT:res = "编译失败，缺少标识符！"; break;
	case LACK_START_L:res = "编译失败，缺少'('！"; break;
	case LACK_END_L:res = "编译失败，缺少')'！"; break;
	case LACK_START_M:res = "编译失败，缺少'['！"; break;
	case LACK_END_M:res = "编译失败，缺少']'！"; break;
	case LACK_BEGIN:res = "编译失败，缺少'{'！"; break;
	case LACK_END:res = "编译失败，缺少'}'！"; break;
	case NESTED_FUNC_DECLEAR:res = "编译失败，函数嵌套定义！"; break;
	case PRIMARY_ERROR:res = "编译失败，初等表达式错误！"; break;
	case RETURN_ERROR:res = "编译失败，return类型错误！"; break;
	case BREAK_LACK_SEMICOLON:res = "编译失败，break缺少';'！"; break;
	case CONTINU_LACK_SEMICOLON:res = "编译失败，continue缺少';'！"; break;
	case IF_LACK_START_L:res = "编译失败，if中缺少'('！"; break;
	case IF_LACK_END_L:res = "编译失败，if中缺少')'!"; break;
	case FOR_LACK_START_L:res = "编译失败，for缺少'('！"; break;
	case FOR_LACK_END_L:res = "编译失败，for缺少')'！"; break;
	case FUNCVAR_WRONG:res = "编译失败，函数调用变量错误！"; break;
	case LACK_ST:res = "编译失败，缺少语句！"; break;
	case CONTINU_ERROR:res = "编译失败，continue使用错误！"; break;
	case ELSE_ERROR:res = "编译失败，else使用错误！"; break;
	case FOR_ERROR:res = "编译失败，for使用错误！"; break;
	case WHILE_LACK_START_L:res = "编译失败，while缺少左圆括号！"; break;
	case WHILE_LACK_END_L:res = "编译失败，while缺少右圆括号！"; break;
	case WHILE_ERROR:res = "编译失败，while使用错误！"; break;
	case IDENTIFY_ERROR:res = "编译失败，标识符不规范！"; break;
	case VAL_EXCESSIVE:res = "编译失败，变量过多！"; break;
	case VAL_ERROR:res = "编译失败，赋值不规范！"; break;
	case PROGRAM_BEGIN:Compile();/*编译入口，上面词法分析未给JmpJud赋值，
		还是默认为0，就进入这个编译入口,当从compile安全退出以后，不加break，直接让他
		往下执行，执行了default再break吧，不然default我也没什么写*/
	default:
		res = "\n==========成功 1 个，失败 0 个，跳过 0 个==========\n";
		break;
	}
	if (SUCC == JmpJud) {
		printf("============已完成项目%s的编译工作=============",argv[1]);
		printf("\n%s", res.c_str());
		return 0;
	}
	else if (SOMETHINGWRONG == JmpJud) {
		printf("\n%s", res.c_str());
	}
	else {
		printf("\n==========%s:%s==========\n", argv[1],res.c_str());
		printf("\n=========== 错误行数：第%d行 ============\n", Row_Error);
	}
}








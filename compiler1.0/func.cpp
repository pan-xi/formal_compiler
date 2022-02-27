#include"func.h"

extern TkWord word[MAXSIZE] ;//用来存分好的词，func.cpp也要用，所以设了全局变量
std::hash<std::string> h;
int row = 1;//用row来控制行号
extern int wordlen;//存入多少结构体了
extern int Row_Error;//存错误行数
extern jmp_buf jmpbuf;//断点
extern int JmpJud;//是否编译成功的标志
pTkWord pword;//始终指向要解析的那个word数组元素
string RT;//return type
stack <string> st;//遇到{,(就压栈,由于str跟char前面词法分析处理过，此处语法分析就不再处理
stack <string> if_st;//有的时候if后面没有else所以if不能跟其他一起压栈,假如压一个{和if，此时没有else让if出栈，那}就不能配对
int finish = 0, err = 0;//finish判断是否完了，err判断是否出错


static TkWord keywords[] = {
{TK_START_L,"("},
{TK_END_L,")"},
{TK_START_M,"["},
{TK_END_M,"]"},
{TK_BEGIN,"{"},
{TK_END,"}"},
{TK_MEMBER,"->"},
{TK_ADD_SELF,"++"},
{TK_DE_SELF,"--"},
{TK_AND,"&"},
{TK_NON,"!"},
{TK_SOF,"sizeof"},
{TK_PLUS,"+"},
{TK_REDUCE,"-"},
{TK_STAR,"*"},
{TK_DIVIDE,"/"},
{TK_MOD,"%"},
{TK_LEFT,"<<"},
{TK_RIGHT,">>"},
{TK_LARGE,">"},
{TK_LITTLE,"<"},
{TK_BQ,">="},
{TK_LQ,"<="},
{TK_EQUL,"=="},
{TK_NEQ,"!="},
{TK_XOR,"^"},
{TK_OR,"|"},
{TK_LAND,"&&"},
{TK_LOR,"||"},
//{TK_CHOICE,"?"},
{TK_GIVEVAL,"="},
{TK_D_GV,"/="},
{TK_M_GV,"*="},
{TK_A_GV,"+="},
{TK_RE_GV,"-="},
{TK_L_GV,"<<="},
{TK_R_GV,">>="},
{TK_AND_GV,"&="},
{TK_X_GV,"^="},
{TK_O_GV,"|="},
{TK_SEMICOLON,";"},
{TK_COMMA,","},
{TK_EOF,"EOF"},
{TK_CNUM,"CNUM"},
//{TK_CFLOAT,"CFLOAT"},
{TK_CSTRING,"CSTRING"},
{TK_CCHAR,"CCHAR"},
{TK_KW_INT,"int"},
{TK_KW_SHORT,"short"},
{TK_KW_CHAR,"char"},
{TK_KW_DOUBLE,"double"},
{TK_KW_FLOAT,"float"},
{TK_KW_LONG,"long"},
{TK_KW_LONGLONG,"long long"},
{TK_KW_CONST,"const"},
{TK_KW_VOID,"void"},
{TK_KW_STRUCT,"struct"},
{TK_KW_FOR,"for"},
{TK_KW_IF,"if"},
{TK_KW_WHILE,"while"},
{TK_KW_ELSE,"else"},
{TK_KW_TYPEDEF,"typedef"},
{TK_KW_DEFINE,"define"},
{TK_KW_INCLUDE,"include"},
{TK_KW_BREAK,"break"},
{TK_KW_RETURN,"return"},
{TK_KW_CONTINU,"continu"},
{TK_KW_ENUM,"enum"},
{TK_MEANING,"//"},
{TK_DEFINE,"define"},
{TK_INCLUDE,"include"},
{0}

};

void word_analysis(FILE * fp) {

	vector <TkWord> hashtable[MAXSIZE];
	int keywordslen = sizeof(keywords) / sizeof(TkWord);
	hash_table(hashtable, keywordslen);
	
	char tmp_c[MAX];//用来接fgets读到的一行
	printbegin();
	while (fgets(tmp_c, MAX, fp)) {//外层循环控制读行，fgets是返回null的
		printf("%-4d", row);
		int i = 0;//i来控制tmp走到哪里
		while (i < strlen(tmp_c))//内层有个循环控制每次取到的tmp里面的字符
		{
			if (tmp_c[i] == ' ' || tmp_c[i] == '\n' || tmp_c[i] == '\t') {
				putchar(tmp_c[i++]);
			}
			else if (tmp_c[i] == '/' && tmp_c[i + 1] == '/') {//注释，未考虑/**/
				meaning_p(&i,tmp_c);
			}
			else if (isalpha(tmp_c[i]) || isunderline(tmp_c[i])) {//标识符关键字sizeof也归为这里
				keyword_p( &i, tmp_c, hashtable);
			}
			else if (isalnum(tmp_c[i])) {//处理数字
				num_p(&i, tmp_c, hashtable);
			}
			else if (tmp_c[i] == '\'')//处理char
			{
				char_p(&i, tmp_c);
			}
			else if (tmp_c[i] == '\"') {//处理字符串
				str_p(&i, tmp_c);
			}
			else {//这个else处理剩下的情况，并且把两个的运算符拿出来单独写,三目运算符没有好想法
				else_p(&i, tmp_c, hashtable);
			}
		}
		PrintWhite();
		row++;
	}
	fclose(fp);

	for (int i = 0; i < wordlen; i++) {//上面把变量，标识符统统赋成变量名，此处把它改过来
		if (word[i].token == TK_VAL && word[i + 1].token == TK_START_L)
			word[i].token = TK_FUNCT;
	}
}


void printbegin() {
	printf("\n——————————simple compiler———————————————\n\n");
}

void hash_table(vector <TkWord> *hashtable, int keywordslen) {
	for (int i = 0; i < keywordslen - 1; i++) {//创建哈希表
		int value = h(keywords[i].s) % MAXSIZE;
		hashtable[value].push_back(keywords[i]);
	}
}

void meaning_p(int *p,char* tmp_c) {
	string tmp_s;
	TkWord tmp_tk;
	int i = *p;
	tmp_s += "//";
	tmp_tk = { TK_MEANING,tmp_s,row };
	word[wordlen++] = tmp_tk;
	//Print(tmp_s, TK_MEANING);//先打印出来//因为注释不存
	i += 2;//存入了//就要跳过前两个，第三个开始存入tmp_s
	while (i < strlen(tmp_c)) {
		tmp_s += tmp_c[i++];//继续存注释
	}
	Print(tmp_s, TK_MEANING);//打印注释
	*p = i;
}

void keyword_p( int* p, char* tmp_c, vector <TkWord>* hashtable) {//标识符关键字sizeof也归为这里
	string tmp_s;
	TkWord tmp_tk;
	int value;
	int i = *p;
	while (isalpha(tmp_c[i]) || isalnum(tmp_c[i]) || isunderline(tmp_c[i]))
	{
		tmp_s += tmp_c[i++];
	}
	tmp_tk = { 0,tmp_s,row };
	value = h(tmp_s) % MAXSIZE;
	int j = 0;
	for (j = 0; j < hashtable[value].size(); j++) {
		if (tmp_tk.s == hashtable[value][j].s) {
			tmp_tk.token = hashtable[value][j].token;//赋关键字，sizeof的token值
			break;
		}
	}
	if (tmp_tk.token == 0) {
		tmp_tk.token = TK_VAL;
	}
	hashtable[value].push_back(tmp_tk);
	word[wordlen++] = tmp_tk;
	Print(tmp_s, tmp_tk.token);
	*p = i;
}

void num_p(int* p, char* tmp_c, vector <TkWord>* hashtable){
	string tmp_s;
	TkWord tmp_tk;
	int value;
	int i = *p;
	int count = 0;//count来处理.的问题
	while (isalnum(tmp_c[i]) || tmp_c[i] == '.' || isalpha(tmp_c[i]))//不断往里读
	{
		if (tmp_c[i] == '.')
		{
			count++;
			if (2 == count) {
				Row_Error = row;
				JmpJud = VAL_ERROR;//
				longjmp(jmpbuf, JmpJud);
			}
		}
		if (isalpha(tmp_c[i])) {
			Row_Error = row;
			//longjmp(jmpbuf, IDENTIFY_ERROR);//若发现字母，就是命名错误
			JmpJud = IDENTIFY_ERROR;//此处longjmp会报错，因为在同一个函数内，所以采用goto
			longjmp(jmpbuf, JmpJud);
		}
		tmp_s += tmp_c[i++];
	}
	tmp_tk = { 0,tmp_s,row };
	value = h(tmp_s) % MAX;
	int j = 0;
	for (j; j < hashtable[value].size(); j++)
		if (tmp_s == hashtable[value][j].s) {
			tmp_tk.token = hashtable[value][j].token;
			break;
		}
	if (j == hashtable[value].size()) {
		tmp_tk.token = TK_CNUM;
		hashtable[value].push_back(tmp_tk);
	}
	word[wordlen++] = tmp_tk;
	Print(tmp_s, tmp_tk.token);
	*p = i;
}

void char_p(int* p, char* tmp_c) {
	string tmp_s;
	TkWord tmp_tk;
	int i = *p;
	for (int j = 0; j < 3; j++) {
		if (j == 3 && tmp_c[i] != '\'') {//暂未考虑char里面处理\的事情
			Row_Error = row;
			//longjmp(jmpbuf, VAL_EXCESSIVE);
			JmpJud = VAL_EXCESSIVE;
			longjmp(jmpbuf, JmpJud);
		}
		tmp_s += tmp_c[i++];

	}
	tmp_tk = { TK_CCHAR,tmp_s,row };
	word[wordlen++] = tmp_tk;
	Print(tmp_s, tmp_tk.token);
	*p = i;
}
void str_p(int* p, char* tmp_c) {
	string tmp_s;
	TkWord tmp_tk;
	int i = *p;
	tmp_s += tmp_c[i++];
	while (tmp_c[i] != '\"') //暂未考虑str里面处理\的事情
	{
		tmp_s += tmp_c[i++];
	}
	tmp_s += tmp_c[i++];
	tmp_tk = { TK_CSTRING,tmp_s,row };//唉如果包装成函数就好了，现在才想起来
	word[wordlen++] = tmp_tk;
	Print(tmp_s, tmp_tk.token);
	*p = i;
}

void next_target(string *tmp_s,char* tmp_c,int *p) {//tmp_s += "++";//已经不适合这么写了，本来想在最后i+2，可还有三个的跟一个的换策略
	int i = *p;
	*tmp_s += tmp_c[i++];
	*tmp_s += tmp_c[i++];
	*p = i;
}

void else_p(int* p, char* tmp_c, vector <TkWord>* hashtable) {
	string tmp_s;
	TkWord tmp_tk;
	tmp_tk.token = 0;
	int value;
	int i = *p;
	if (tmp_c[i] == '+' && tmp_c[i + 1] == '+') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_ADD_SELF;
	}
	else if (tmp_c[i] == '-' && tmp_c[i + 1] == '-') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_DE_SELF;
	}
	else if (tmp_c[i] == '<' && tmp_c[i + 1] == '<' && tmp_c[i + 2] == '=') {//这个放<<前面可以先判断<<=避免先判断<<
		next_target(&tmp_s, tmp_c, &i);
		tmp_s += tmp_c[i++];
		tmp_tk.token = TK_L_GV;
	}
	else if (tmp_c[i] == '>' && tmp_c[i + 1] == '>' && tmp_c[i + 2] == '=') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_s += tmp_c[i++];
		tmp_tk.token = TK_R_GV;
	}
	else if (tmp_c[i] == '<' && tmp_c[i + 1] == '<') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_LEFT;
	}
	else if (tmp_c[i] == '>' && tmp_c[i + 1] == '>') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_RIGHT;
	}
	else if (tmp_c[i] == '>' && tmp_c[i + 1] == '=') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_BQ;
	}
	else if (tmp_c[i] == '<' && tmp_c[i + 1] == '=') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_LQ;
	}
	else if (tmp_c[i] == '=' && tmp_c[i + 1] == '=') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_EQUL;
	}
	else if (tmp_c[i] == '!' && tmp_c[i + 1] == '=') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_NEQ;
	}
	else if (tmp_c[i] == '&' && tmp_c[i + 1] == '&') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_LAND;
	}
	else if (tmp_c[i] == '|' && tmp_c[i + 1] == '|') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_LOR;
	}
	else if (tmp_c[i] == '/' && tmp_c[i + 1] == '=') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_D_GV;
	}
	else if (tmp_c[i] == '*' && tmp_c[i + 1] == '=') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_M_GV;
	}
	else if (tmp_c[i] == '+' && tmp_c[i + 1] == '=') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_A_GV;
	}
	else if (tmp_c[i] == '-' && tmp_c[i + 1] == '=') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_RE_GV;
	}
	else if (tmp_c[i] == '&' && tmp_c[i + 1] == '=') {
		next_target(&tmp_s, tmp_c, &i);;
		tmp_tk.token = TK_AND_GV;
	}
	else if (tmp_c[i] == '^' && tmp_c[i + 1] == '=') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_X_GV;
	}
	else if (tmp_c[i] == '|' && tmp_c[i + 1] == '=') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_O_GV;
	}
	else if (tmp_c[i] == '-' && tmp_c[i + 1] == '>') {
		next_target(&tmp_s, tmp_c, &i);
		tmp_tk.token = TK_MEMBER;
	}
	else {//其他运算符
		tmp_s += tmp_c[i++];
	}
	tmp_tk = { tmp_tk.token,tmp_s,row };//统一存入了
	if (!tmp_tk.token) {//当token=0说明走的是最后一个else，token还没赋值
		value = h(tmp_s) % MAXSIZE;
		int j = 0;
		for (j; j < hashtable[value].size(); j++)
			if (tmp_s == hashtable[value][j].s) {
				tmp_tk.token = hashtable[value][j].token;
				break;
			}
	}
	word[wordlen++] = tmp_tk;
	Print(tmp_s, tmp_tk.token);
	*p = i;
}

//打印颜色
void Print(string ch, int token) {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);//一个句柄，刀柄
	if (token >= TK_FUNCT)
		SetConsoleTextAttribute(h, FOREGROUND_INTENSITY);//函数和变量灰色
	else if (token >= TK_KW_INT)
		SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_INTENSITY);//注释和关键字绿色
	else if (token >= TK_CNUM)
		SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN);//常量褐色
	else
		SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_INTENSITY);
	if (-1 == ch[0]) {
		printf("\n	End_Of_File!");
		SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	}
	cout << ch;
}

void PrintWhite() {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}



//语法分析入口
void Compile(){
	JmpJud = SUCC;
	pword = word;
	External_Claim();//先进入外部声明
}

//外部声明
void External_Claim() {//没写判断}
	if (pword == word + wordlen) {//word + wordlen指向最后一个元素，如果pword相等证明走到最后了
		longjmp(jmpbuf,SUCC);//跳回去并传succ
	}
	else if (TK_MEANING == pword->token) {//注释，都存一起了，注释一整行都是在一个struct里
	}
	else if ("#" == pword->s) {//是不是导入包
		if (TK_DEFINE == (pword+1)->token ) {
			//int define_count;//计数，计define
			pword += 4;//跳过# define xx xx
		}
		else if (TK_INCLUDE == (pword + 1)->token) {
			while (pword->token != TK_LARGE) {
				pword++;
			}
		}
		else {
			return_error(EXTER_DEC);
		}
	}
	else if (!istype) {//不是类型声明
		return_error(EXTER_DEC);
	}
	else {
		type_judge();
	}
	pword++;
	External_Claim();
}


void type_judge() {
	if (TK_VAL == (pword + 1)->token) {
		int i = pword->token;
		RT = pword->token;//存入函数返回值类型
		pword += 2;//拿过类型和变量
		val_process(i);
	}
	else if (TK_FUNCT == (pword + 1)->token) {
		RT = pword->s;
		pword += 2;//跳过 类型与函数名
		func_process();
	}
	else if (TK_KW_TYPEDEF == pword->token) {
		//typedef_process();
		pword++;
		if (TK_KW_STRUCT == pword ->token) {
			
			typedef_struct();
		}
		else {
			typedef_name();
		}

	}
	else {
		return_error(EXTER_DEC);
	}
}


void val_process(int i) {
	if (TK_SEMICOLON==pword->token) {//;就推出
		return;
	}
	else if (TK_GIVEVAL == pword->token) {
		pword++;
		switch (i) {
		case TK_KW_INT:
			if (TK_CNUM == pword->token) {
				pword++;
				val_process(i);//递归调用自己
			}
			else {
				return_error(VAL_ERROR);
			}
			break;
		case TK_KW_SHORT:
			if (TK_CNUM == pword->token) {
				pword++;
				val_process(i);//递归调用自己
			}
			else {
				return_error(VAL_ERROR);
			}
			break;
		case TK_KW_DOUBLE:
			if (TK_CNUM == pword->token) {
				pword++;
				val_process(i);//递归调用自己
			}
			else {
				return_error(VAL_ERROR);
			}
			break;
		case TK_KW_FLOAT:
			if (TK_CNUM == pword->token) {
				pword++;
				val_process(i);//递归调用自己
			}
			else {
				return_error(VAL_ERROR);
			}
			break;
		case TK_KW_LONG:
			if (TK_CNUM == pword->token) {
				pword++;
				val_process(i);//递归调用自己
			}
			else {
				return_error(VAL_ERROR);
			}
			break;
		case TK_KW_LONGLONG:
			if (TK_CNUM == pword->token) {
				pword++;
				val_process(i);//递归调用自己
			}
			else {
				return_error(VAL_ERROR);
			}
			break;
		case TK_KW_CHAR:
			if (TK_CCHAR == pword->token) {
				pword++;
				val_process(i);//递归调用自己
			}
			else {
				return_error(VAL_ERROR);
			}
			break;
		default:
			return_error(VAL_WRONG);
		}
	}
	else if(TK_COMMA==pword->token)
	{
		pword++;
		if (TK_VAL != pword->token) {
			return_error(VAL_WRONG);
		}
		else {
			pword++;
			val_process(i);
		}
	}
	else{
		return_error(VAL_WRONG);
	}
}

void func_process() {
	if (TK_START_L != pword->token) {//不是（
		return_error(LACK_START_L);
	}
	else//是（
	{
		pword++;
		if (TK_END_L == pword->token) {
			pword++;
		}
		else {//不是）就要处理形参
			func_val_process();
			pword++;
		}
		if (TK_SEMICOLON == pword->token) {//是函数声明
			return;
		}
		else if (TK_BEGIN == pword->token) {//如果是{，则开始处理函数体内部
			func_body_process();
			return;
		}
		else {
			return_error(FUNC_WRONG);
		}
	}
}

void func_val_process() {
	if (!istype) {
		return_error(FUNC_WRONG);
	}
	else {
		pword++;
		if (pword->token!=TK_VAL){
			return_error(FUNC_WRONG);
		}
		pword++;
		if (TK_END_L == pword->token) {
			return;
		}
		else if (TK_COMMA == pword->token) {
			pword++;
			func_val_process();//递归处理形参
		}
		else {
			return_error(FUNC_WRONG);
		}
	}
}
void func_body_process() {//此时pword指向{,此处do while还暂未处理，考虑到与while类似，下个版本更新
	st.push(pword->s);
	pword++;
	if (TK_END == pword->token) {
		st.pop();
		return;
	}
	else {
		while (!st.empty()) {
			switch (pword->token) {//此时指向{里的第一个元素
			case TK_MEANING:
				pword++;//因为仅存一个"//"在结构体内，而后面的注释不存的
				break;
			case TK_KW_CHAR:
				pword++;
				func_char();
				break;
			case TK_KW_INT:
				pword++;
				func_num();
				pword++;
				break;
			case TK_KW_SHORT:
				pword++;
				func_num();
				break;
			case TK_KW_DOUBLE:
				pword++;
				func_num();
				break;
			case TK_KW_FLOAT:
				pword++;
				func_num();
				break;
			case TK_KW_LONG:
				pword++;
				func_num();
				break;
			case TK_KW_LONGLONG:
				pword++;
				func_num();
				break;
			case TK_KW_FOR:
				for_process();
				break;
			case TK_KW_IF:
				if_process();
				break;
			case TK_KW_WHILE:
				while_process();
				break;
			case TK_KW_ELSE:
				else_process();
				break;
			case TK_KW_BREAK:
				pword++;
				if (TK_SEMICOLON != pword->token) {
					return_error(BREAK_LACK_SEMICOLON);
				}
				pword++;
				break;
			case TK_KW_RETURN:
				pword++;
				return_process();
				break; 
			case TK_KW_CONTINU:
				pword++;
				if (TK_SEMICOLON != pword->token) {
					return_error(CONTINU_LACK_SEMICOLON);
				}
				pword++;
				break;
			case TK_KW_ENUM:
				enum_process();
				break;
			case TK_BEGIN:
				st.push(pword->s);
				pword++;
				break;
			case TK_END:
				if (st.top() == "{") {
					st.pop();
					if (!st.empty()) {
						pword++;
					}
					break;
				}
				else {
					return_error(LACK_BEGIN);
				}
				break;
			case TK_VAL:
				primary_analysis();
				pword++;
				break;
			case TK_FUNCT:
				func_func();
				break;
			default:
				return_error(SOMETHINGWRONG);
				break;
			}
		}
	}
	while (!st.empty()) {
		st.pop();
	}
	while (!if_st.empty()) {
		if_st.pop();
	}
}
void typedef_struct() {//此时pword指向的是struct
	pword++;
	if (TK_BEGIN != pword->token) {
		pword++;
	}//此时，pword指向{
	pword++;
	int i;
	while (TK_END != pword->token) {
		i = pword->token;
		val_process(i);//借处理外部声明的变量去分析这个变量，反正都一样
	}
}

void typedef_name() {//此时指向typedef后面的那个
	pword += 2;
	if (TK_SEMICOLON != pword->token) {
		return_error(EXTER_DEC);
	}
	pword++;
}

void func_char() {//此时指向char后一个元素,暂不考虑赋值的时候赋一个表达式问题
	if (TK_VAL == pword->token) {
		pword++;
		if (TK_GIVEVAL == pword->token) {
			pword++;
			if (TK_CCHAR == pword->token || TK_CNUM == pword->token)//败笔，前面没把float与int分开，将错就错吧
			{
				pword++;
				if (TK_COMMA == pword->token) {
					pword++;
					func_char();
				}
				else if (TK_SEMICOLON == pword->token) {
					pword++;
					return;
				}
				else {
					return_error(VAL_WRONG);
				}
			}
			else{
				return_error(VAL_WRONG);
			}
		}
		else if (TK_COMMA == pword->token) {
			pword++;
			func_char();
		}
		else if (TK_SEMICOLON == pword->token) {
			pword++;
			return;
		}
		else {
			return_error(VAL_WRONG);
		}
	}
	else {
		return_error(VAL_WRONG);
	}
	
}
void func_num() {//此时指到int下一个
	if (TK_VAL == pword->token) {
		primary_analysis();
		while ("," == pword->s&&TK_VAL==(pword+1)->token) {
			pword++;//如果是,则跳过,
			primary_analysis();
		}
	}
	else {
		return_error(VAL_WRONG);
	}

}
void for_process() {//此时指向for
	pword++;
	int count=0;//计数，表达式个数
	if (TK_START_L != pword->token) {
		return_error(FOR_LACK_START_L);
	}
	pword++;
	if (TK_SEMICOLON == pword->token) {
		count++;
		pword++;
	}
	else {
		if (istype) {//缺陷暂未处理，没考虑上面定义过变量不需要重新声明这件事
			pword++;
		}
		primary_analysis();
		if (TK_SEMICOLON != pword->token) {
			return_error(FOR_ERROR);
		}
		count++;
		pword++;
	}//第一个表达式处理完，此时指向;的下一个
	if (TK_SEMICOLON == pword->token) {
		count++;
		pword++;
	}
	else {
		primary_analysis();
		if (TK_SEMICOLON != pword->token) {
			return_error(FOR_ERROR);
		}
		count++;
		pword++;
	}//第二个表达式处理完
	if (2 != count) {//前两个表达式不全
		return_error(FOR_ERROR);
	}
	if (TK_END_L == pword->token) {
		pword++;
	}
	else {
		primary_analysis();
		if (TK_END_L != pword->token) {
			return_error(FOR_LACK_END_L);
		}
		pword++;
	}//for的表达式处理完成，接下来处理有{的情况与无{的情况
	if (TK_SEMICOLON == pword->token) {
		pword++;
	}
	else if (TK_BEGIN == pword->token) {
		st.push(pword->s);//遇到{压栈
		pword++;
	}
	else if (TK_VAL == pword->token) {
		primary_analysis();
		if (TK_SEMICOLON != pword->token) {
			return_error(FOR_ERROR);
		}
	}
	else {
		return_error(FOR_ERROR);
	}

}
void if_process() {//指向if
	if_st.push(pword->s);
	pword++;
	if (TK_START_L != pword->token) {
		return_error(IF_LACK_START_L);
	}
	pword++;
	primary_analysis();
	if (TK_END_L != pword->token) {
		return_error(IF_LACK_END_L);
	}
	pword++;
	if (TK_BEGIN == pword->token) {
		st.push(pword->s);
		pword++;
	}
	else if (TK_SEMICOLON == pword->token) {
		pword++;
	}
	else if(TK_END==pword->token){
		return_error(LACK_ST);
	}
	else {
		return;//if逻辑很乱有待再次整理
	}
}
void while_process() {//pword->s==while
	pword++;
	if (TK_START_L != pword->token) {//不用if（==） else的原因：少些一点嵌套
		return_error(WHILE_LACK_START_L);
	}
	pword++;
	if (TK_END_L == pword->token) {
		return_error(WHILE_ERROR);
	}
	primary_analysis();
	if (TK_END_L != pword->token) {
		return_error(WHILE_LACK_END_L);
	}//接下来处理有没有{}
	pword++;
	if (TK_BEGIN == pword->token) {
		st.push(pword->s);
		pword++;
	}
	else if ( TK_END==pword->token ) {
		return_error(LACK_ST);
	}
	else {
		return;
	}


	
}
void else_process() {//pword->s==else
	if (if_st.empty()) {
		return_error(ELSE_ERROR);
	}
	if_st.pop();
	pword++;
	if (TK_BEGIN == pword->token) {
		st.push(pword->s);
		pword++;
	}
	else if (TK_END == pword->token) {
		return_error(LACK_ST);
	}
	else {
		return;
	}
}

void return_process() {//return也有问题，没有充分考虑到返回值类型，下次把结构体多加一个成员
	if (RT == "void") {
		if (TK_SEMICOLON== pword->token ) {
			pword++;
		}
		else {
			return_error(RETURN_ERROR);
		}
	}
	else {
		if (TK_CNUM ==pword->token ||TK_VAL  == pword->token) {
			pword++;
			if (pword->token != TK_SEMICOLON) {
				return_error(RETURN_ERROR);
			}
			pword++;
		}
		else {
			return_error(RETURN_ERROR);
		}
	}
}
void enum_process() {//pword->s==enum
	pword+=2;
	if (TK_BEGIN != pword->token)
	{
		return_error(LACK_BEGIN);
	}
	pword++;
	while (TK_VAL == pword->token) {//暂未处理标识符相同的，以及未给val赋值，还需要一个结构体
		pword += 2;
	}
	if (TK_END != pword->token) {
		return_error(LACK_END);
	}
	pword++;
	if (TK_SEMICOLON != pword->token) {
		return_error(LACK_SEMICOLON);
	}
	pword++;
	return;
}

void func_func() {//掉用函数,暂未考虑传参类型匹配问题，下次存个函数及对应地址
	pword++;
	if (TK_START_L != pword->token) {
		return_error(LACK_START_L);
	}
	pword++;
	if (TK_END_L == pword->token) {
		pword++;
		if (TK_SEMICOLON != pword->token) {
			return_error(LACK_SEMICOLON);
		}
		pword++;
		return;
	}
	else {
		func_val();
	}
}
void func_val() {
	if (TK_CNUM == pword->token || TK_CCHAR == pword->token || TK_CSTRING == pword->token || TK_VAL == pword->token) {
		pword++;
		if (TK_COMMA == pword->token) {
			pword++;
			func_val();
		}
		else if (TK_END_L == pword->token) {
			pword++;
			if (TK_SEMICOLON != pword->token) {
				return_error(LACK_SEMICOLON);
			}
			pword++;
			return;
		}
		else {
			return_error(LACK_END_L);
		}
	}
	else {
		return_error(FUNCVAR_WRONG);
	}
}
void primary_analysis() {//PWORD->TOKEN==VAL
	C();
	if (finish == 1 && err == 0) {
		return;
	}
	else{
		return_error(PRIMARY_ERROR);
	}
}
void C() {//INIT,初等表达式
	S();
	while (TK_COMMA == pword->token) {
		next();
		S();
	}
	
}
void next() {
	pword++;
	if (TK_SEMICOLON == pword->token) {
		finish = 1;
	}
}
void S() {//赋值
	L();
	while (pword->token <= TK_O_GV && pword->token >= TK_GIVEVAL) {
		next();
		L();
	}
}
void L() {//逻辑运算符，条件运算符暂时处理不来
	J();
	while (pword->token >= TK_AND && pword->token <= TK_LOR) {
		next();
		J();
	}
}
void J() {//judge
	M();
	while (pword->token >= TK_LARGE && pword->token <= TK_NEQ) {
		next();
		M();
	}
}
void M() {//move
	P();
	while (pword->token == TK_LEFT || pword->token == TK_RIGHT) {
		next();
		P();
	}
}
void P() {//+-
	H();
	while (pword->token == TK_PLUS || pword->token == TK_REDUCE) {
		next();
		H();
	}
}
void H() {// * / %
	T();
	while (pword->token >= TK_STAR && pword->token <= TK_MOD) {
		next();
		T();
	}
}
void T() {//
	U();
	while (pword->token >= TK_ADD_SELF && pword->token <= TK_SOF) {
		next();
		U();
	}
}
void U() {//ultra
	if (pword->token == TK_CNUM || pword->token == TK_VAL) {
		next();
	}
	else if (TK_BEGIN == pword->token) {
		next();
		C();
		if (TK_START_M == pword->token) {
			next();
			C();
			if (TK_START_L == pword->token) {
				next();
				C();
			}
			if (TK_END_L == pword->token) {
				next();
			}
			else {
				err = -1;
				Row_Error = pword->row;
			}
		}
		if (TK_END_M == pword->token) {
			next();
		}
		else {
			err = -1;
			Row_Error = pword->row;
		}
	
	if (TK_END == pword->token) {
		next();
	}
	else {
		err = -1;
		Row_Error = pword->row;
	}
	}
	else if (TK_START_M == pword->token) {
		next();
		C();
		if (TK_START_L == pword->token) {
			next();
			C();
		}
		if (TK_END_L == pword->token) {
			next();
		}
		else {
			err = -1;
			Row_Error = pword->row;
		}
	
	if (TK_END_M == pword->token) {
		next();
	}
	else {
		err = -1;
		Row_Error = pword->row;
	}
	}
	else if (TK_START_L == pword->token) {
		next();
		C();
	
	if (TK_END_L == pword->token) {
		next();
	}
	else {
		err = -1;
		Row_Error = pword->row;
	}
	}

}
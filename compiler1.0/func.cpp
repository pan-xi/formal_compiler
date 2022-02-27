#include"func.h"

extern TkWord word[MAXSIZE] ;//������ֺõĴʣ�func.cppҲҪ�ã���������ȫ�ֱ���
std::hash<std::string> h;
int row = 1;//��row�������к�
extern int wordlen;//������ٽṹ����
extern int Row_Error;//���������
extern jmp_buf jmpbuf;//�ϵ�
extern int JmpJud;//�Ƿ����ɹ��ı�־
pTkWord pword;//ʼ��ָ��Ҫ�������Ǹ�word����Ԫ��
string RT;//return type
stack <string> st;//����{,(��ѹջ,����str��charǰ��ʷ�������������˴��﷨�����Ͳ��ٴ���
stack <string> if_st;//�е�ʱ��if����û��else����if���ܸ�����һ��ѹջ,����ѹһ��{��if����ʱû��else��if��ջ����}�Ͳ������
int finish = 0, err = 0;//finish�ж��Ƿ����ˣ�err�ж��Ƿ����


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
	
	char tmp_c[MAX];//������fgets������һ��
	printbegin();
	while (fgets(tmp_c, MAX, fp)) {//���ѭ�����ƶ��У�fgets�Ƿ���null��
		printf("%-4d", row);
		int i = 0;//i������tmp�ߵ�����
		while (i < strlen(tmp_c))//�ڲ��и�ѭ������ÿ��ȡ����tmp������ַ�
		{
			if (tmp_c[i] == ' ' || tmp_c[i] == '\n' || tmp_c[i] == '\t') {
				putchar(tmp_c[i++]);
			}
			else if (tmp_c[i] == '/' && tmp_c[i + 1] == '/') {//ע�ͣ�δ����/**/
				meaning_p(&i,tmp_c);
			}
			else if (isalpha(tmp_c[i]) || isunderline(tmp_c[i])) {//��ʶ���ؼ���sizeofҲ��Ϊ����
				keyword_p( &i, tmp_c, hashtable);
			}
			else if (isalnum(tmp_c[i])) {//��������
				num_p(&i, tmp_c, hashtable);
			}
			else if (tmp_c[i] == '\'')//����char
			{
				char_p(&i, tmp_c);
			}
			else if (tmp_c[i] == '\"') {//�����ַ���
				str_p(&i, tmp_c);
			}
			else {//���else����ʣ�µ���������Ұ�������������ó�������д,��Ŀ�����û�к��뷨
				else_p(&i, tmp_c, hashtable);
			}
		}
		PrintWhite();
		row++;
	}
	fclose(fp);

	for (int i = 0; i < wordlen; i++) {//����ѱ�������ʶ��ͳͳ���ɱ��������˴������Ĺ���
		if (word[i].token == TK_VAL && word[i + 1].token == TK_START_L)
			word[i].token = TK_FUNCT;
	}
}


void printbegin() {
	printf("\n��������������������simple compiler������������������������������\n\n");
}

void hash_table(vector <TkWord> *hashtable, int keywordslen) {
	for (int i = 0; i < keywordslen - 1; i++) {//������ϣ��
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
	//Print(tmp_s, TK_MEANING);//�ȴ�ӡ����//��Ϊע�Ͳ���
	i += 2;//������//��Ҫ����ǰ��������������ʼ����tmp_s
	while (i < strlen(tmp_c)) {
		tmp_s += tmp_c[i++];//������ע��
	}
	Print(tmp_s, TK_MEANING);//��ӡע��
	*p = i;
}

void keyword_p( int* p, char* tmp_c, vector <TkWord>* hashtable) {//��ʶ���ؼ���sizeofҲ��Ϊ����
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
			tmp_tk.token = hashtable[value][j].token;//���ؼ��֣�sizeof��tokenֵ
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
	int count = 0;//count������.������
	while (isalnum(tmp_c[i]) || tmp_c[i] == '.' || isalpha(tmp_c[i]))//���������
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
			//longjmp(jmpbuf, IDENTIFY_ERROR);//��������ĸ��������������
			JmpJud = IDENTIFY_ERROR;//�˴�longjmp�ᱨ����Ϊ��ͬһ�������ڣ����Բ���goto
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
		if (j == 3 && tmp_c[i] != '\'') {//��δ����char���洦��\������
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
	while (tmp_c[i] != '\"') //��δ����str���洦��\������
	{
		tmp_s += tmp_c[i++];
	}
	tmp_s += tmp_c[i++];
	tmp_tk = { TK_CSTRING,tmp_s,row };//�������װ�ɺ����ͺ��ˣ����ڲ�������
	word[wordlen++] = tmp_tk;
	Print(tmp_s, tmp_tk.token);
	*p = i;
}

void next_target(string *tmp_s,char* tmp_c,int *p) {//tmp_s += "++";//�Ѿ����ʺ���ôд�ˣ������������i+2���ɻ��������ĸ�һ���Ļ�����
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
	else if (tmp_c[i] == '<' && tmp_c[i + 1] == '<' && tmp_c[i + 2] == '=') {//�����<<ǰ��������ж�<<=�������ж�<<
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
	else {//���������
		tmp_s += tmp_c[i++];
	}
	tmp_tk = { tmp_tk.token,tmp_s,row };//ͳһ������
	if (!tmp_tk.token) {//��token=0˵���ߵ������һ��else��token��û��ֵ
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

//��ӡ��ɫ
void Print(string ch, int token) {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);//һ�����������
	if (token >= TK_FUNCT)
		SetConsoleTextAttribute(h, FOREGROUND_INTENSITY);//�����ͱ�����ɫ
	else if (token >= TK_KW_INT)
		SetConsoleTextAttribute(h, FOREGROUND_GREEN | FOREGROUND_INTENSITY);//ע�ͺ͹ؼ�����ɫ
	else if (token >= TK_CNUM)
		SetConsoleTextAttribute(h, FOREGROUND_RED | FOREGROUND_GREEN);//������ɫ
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



//�﷨�������
void Compile(){
	JmpJud = SUCC;
	pword = word;
	External_Claim();//�Ƚ����ⲿ����
}

//�ⲿ����
void External_Claim() {//ûд�ж�}
	if (pword == word + wordlen) {//word + wordlenָ�����һ��Ԫ�أ����pword���֤���ߵ������
		longjmp(jmpbuf,SUCC);//����ȥ����succ
	}
	else if (TK_MEANING == pword->token) {//ע�ͣ�����һ���ˣ�ע��һ���ж�����һ��struct��
	}
	else if ("#" == pword->s) {//�ǲ��ǵ����
		if (TK_DEFINE == (pword+1)->token ) {
			//int define_count;//��������define
			pword += 4;//����# define xx xx
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
	else if (!istype) {//������������
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
		RT = pword->token;//���뺯������ֵ����
		pword += 2;//�ù����ͺͱ���
		val_process(i);
	}
	else if (TK_FUNCT == (pword + 1)->token) {
		RT = pword->s;
		pword += 2;//���� �����뺯����
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
	if (TK_SEMICOLON==pword->token) {//;���Ƴ�
		return;
	}
	else if (TK_GIVEVAL == pword->token) {
		pword++;
		switch (i) {
		case TK_KW_INT:
			if (TK_CNUM == pword->token) {
				pword++;
				val_process(i);//�ݹ�����Լ�
			}
			else {
				return_error(VAL_ERROR);
			}
			break;
		case TK_KW_SHORT:
			if (TK_CNUM == pword->token) {
				pword++;
				val_process(i);//�ݹ�����Լ�
			}
			else {
				return_error(VAL_ERROR);
			}
			break;
		case TK_KW_DOUBLE:
			if (TK_CNUM == pword->token) {
				pword++;
				val_process(i);//�ݹ�����Լ�
			}
			else {
				return_error(VAL_ERROR);
			}
			break;
		case TK_KW_FLOAT:
			if (TK_CNUM == pword->token) {
				pword++;
				val_process(i);//�ݹ�����Լ�
			}
			else {
				return_error(VAL_ERROR);
			}
			break;
		case TK_KW_LONG:
			if (TK_CNUM == pword->token) {
				pword++;
				val_process(i);//�ݹ�����Լ�
			}
			else {
				return_error(VAL_ERROR);
			}
			break;
		case TK_KW_LONGLONG:
			if (TK_CNUM == pword->token) {
				pword++;
				val_process(i);//�ݹ�����Լ�
			}
			else {
				return_error(VAL_ERROR);
			}
			break;
		case TK_KW_CHAR:
			if (TK_CCHAR == pword->token) {
				pword++;
				val_process(i);//�ݹ�����Լ�
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
	if (TK_START_L != pword->token) {//���ǣ�
		return_error(LACK_START_L);
	}
	else//�ǣ�
	{
		pword++;
		if (TK_END_L == pword->token) {
			pword++;
		}
		else {//���ǣ���Ҫ�����β�
			func_val_process();
			pword++;
		}
		if (TK_SEMICOLON == pword->token) {//�Ǻ�������
			return;
		}
		else if (TK_BEGIN == pword->token) {//�����{����ʼ���������ڲ�
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
			func_val_process();//�ݹ鴦���β�
		}
		else {
			return_error(FUNC_WRONG);
		}
	}
}
void func_body_process() {//��ʱpwordָ��{,�˴�do while����δ�������ǵ���while���ƣ��¸��汾����
	st.push(pword->s);
	pword++;
	if (TK_END == pword->token) {
		st.pop();
		return;
	}
	else {
		while (!st.empty()) {
			switch (pword->token) {//��ʱָ��{��ĵ�һ��Ԫ��
			case TK_MEANING:
				pword++;//��Ϊ����һ��"//"�ڽṹ���ڣ��������ע�Ͳ����
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
void typedef_struct() {//��ʱpwordָ�����struct
	pword++;
	if (TK_BEGIN != pword->token) {
		pword++;
	}//��ʱ��pwordָ��{
	pword++;
	int i;
	while (TK_END != pword->token) {
		i = pword->token;
		val_process(i);//�账���ⲿ�����ı���ȥ�������������������һ��
	}
}

void typedef_name() {//��ʱָ��typedef������Ǹ�
	pword += 2;
	if (TK_SEMICOLON != pword->token) {
		return_error(EXTER_DEC);
	}
	pword++;
}

void func_char() {//��ʱָ��char��һ��Ԫ��,�ݲ����Ǹ�ֵ��ʱ��һ�����ʽ����
	if (TK_VAL == pword->token) {
		pword++;
		if (TK_GIVEVAL == pword->token) {
			pword++;
			if (TK_CCHAR == pword->token || TK_CNUM == pword->token)//�ܱʣ�ǰ��û��float��int�ֿ�������ʹ��
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
void func_num() {//��ʱָ��int��һ��
	if (TK_VAL == pword->token) {
		primary_analysis();
		while ("," == pword->s&&TK_VAL==(pword+1)->token) {
			pword++;//�����,������,
			primary_analysis();
		}
	}
	else {
		return_error(VAL_WRONG);
	}

}
void for_process() {//��ʱָ��for
	pword++;
	int count=0;//���������ʽ����
	if (TK_START_L != pword->token) {
		return_error(FOR_LACK_START_L);
	}
	pword++;
	if (TK_SEMICOLON == pword->token) {
		count++;
		pword++;
	}
	else {
		if (istype) {//ȱ����δ����û�������涨�����������Ҫ�������������
			pword++;
		}
		primary_analysis();
		if (TK_SEMICOLON != pword->token) {
			return_error(FOR_ERROR);
		}
		count++;
		pword++;
	}//��һ�����ʽ�����꣬��ʱָ��;����һ��
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
	}//�ڶ������ʽ������
	if (2 != count) {//ǰ�������ʽ��ȫ
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
	}//for�ı��ʽ������ɣ�������������{���������{�����
	if (TK_SEMICOLON == pword->token) {
		pword++;
	}
	else if (TK_BEGIN == pword->token) {
		st.push(pword->s);//����{ѹջ
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
void if_process() {//ָ��if
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
		return;//if�߼������д��ٴ�����
	}
}
void while_process() {//pword->s==while
	pword++;
	if (TK_START_L != pword->token) {//����if��==�� else��ԭ����Щһ��Ƕ��
		return_error(WHILE_LACK_START_L);
	}
	pword++;
	if (TK_END_L == pword->token) {
		return_error(WHILE_ERROR);
	}
	primary_analysis();
	if (TK_END_L != pword->token) {
		return_error(WHILE_LACK_END_L);
	}//������������û��{}
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

void return_process() {//returnҲ�����⣬û�г�ֿ��ǵ�����ֵ���ͣ��´ΰѽṹ����һ����Ա
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
	while (TK_VAL == pword->token) {//��δ�����ʶ����ͬ�ģ��Լ�δ��val��ֵ������Ҫһ���ṹ��
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

void func_func() {//���ú���,��δ���Ǵ�������ƥ�����⣬�´δ����������Ӧ��ַ
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
void C() {//INIT,���ȱ��ʽ
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
void S() {//��ֵ
	L();
	while (pword->token <= TK_O_GV && pword->token >= TK_GIVEVAL) {
		next();
		L();
	}
}
void L() {//�߼�������������������ʱ������
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
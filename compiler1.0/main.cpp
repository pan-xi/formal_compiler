#include"func.h"
//#define MAXSZIE 1000
TkWord word[MAXSIZE] = { 0 };//������ֺõĴʣ�func.cppҲҪ�ã���������ȫ�ֱ���
int wordlen;//������ٽṹ����
int Row_Error;//���������
jmp_buf jmpbuf;//�ϵ�
int JmpJud;//�Ƿ����ɹ��ı�־



int main(int argc,char*argv[]) {
	if (argc != 2) {
		printf("�봫��һ���ļ�");
			return -1;
	}
	FILE* fp = fopen(argv[1], "r");//����Ҫ������ļ�����׳��ǿһ��
	if (NULL == fp) {
		perror("fopen");
	}
	word_analysis(fp);//����ʷ�����

	/*�˴��ʷ������ѽ���
	��������﷨����*/
	
	JmpJud = setjmp(jmpbuf);
	string res;//��Ҫ��ӡ���ַ�
	

	switch (JmpJud)
	{
	case SOMETHINGWRONG:res = "����ʧ�ܣ�δ֪����"; break;
	case LACK_SEMICOLON:res = "����ʧ�ܣ�ȱ��';'��"; break;
	case EXTER_DEC:res = "����ʧ�ܣ��ⲿ������������"; break;
	case LACK_TYPE:res = "����ʧ�ܣ�ȱ�����ͷ���"; break;
	case VAL_WRONG:res = "����ʧ�ܣ���������"; break;
	case FUNC_WRONG:res = "����ʧ�ܣ���������"; break;
	case LACK_IDENT:res = "����ʧ�ܣ�ȱ�ٱ�ʶ����"; break;
	case LACK_START_L:res = "����ʧ�ܣ�ȱ��'('��"; break;
	case LACK_END_L:res = "����ʧ�ܣ�ȱ��')'��"; break;
	case LACK_START_M:res = "����ʧ�ܣ�ȱ��'['��"; break;
	case LACK_END_M:res = "����ʧ�ܣ�ȱ��']'��"; break;
	case LACK_BEGIN:res = "����ʧ�ܣ�ȱ��'{'��"; break;
	case LACK_END:res = "����ʧ�ܣ�ȱ��'}'��"; break;
	case NESTED_FUNC_DECLEAR:res = "����ʧ�ܣ�����Ƕ�׶��壡"; break;
	case PRIMARY_ERROR:res = "����ʧ�ܣ����ȱ��ʽ����"; break;
	case RETURN_ERROR:res = "����ʧ�ܣ�return���ʹ���"; break;
	case BREAK_LACK_SEMICOLON:res = "����ʧ�ܣ�breakȱ��';'��"; break;
	case CONTINU_LACK_SEMICOLON:res = "����ʧ�ܣ�continueȱ��';'��"; break;
	case IF_LACK_START_L:res = "����ʧ�ܣ�if��ȱ��'('��"; break;
	case IF_LACK_END_L:res = "����ʧ�ܣ�if��ȱ��')'!"; break;
	case FOR_LACK_START_L:res = "����ʧ�ܣ�forȱ��'('��"; break;
	case FOR_LACK_END_L:res = "����ʧ�ܣ�forȱ��')'��"; break;
	case FUNCVAR_WRONG:res = "����ʧ�ܣ��������ñ�������"; break;
	case LACK_ST:res = "����ʧ�ܣ�ȱ����䣡"; break;
	case CONTINU_ERROR:res = "����ʧ�ܣ�continueʹ�ô���"; break;
	case ELSE_ERROR:res = "����ʧ�ܣ�elseʹ�ô���"; break;
	case FOR_ERROR:res = "����ʧ�ܣ�forʹ�ô���"; break;
	case WHILE_LACK_START_L:res = "����ʧ�ܣ�whileȱ����Բ���ţ�"; break;
	case WHILE_LACK_END_L:res = "����ʧ�ܣ�whileȱ����Բ���ţ�"; break;
	case WHILE_ERROR:res = "����ʧ�ܣ�whileʹ�ô���"; break;
	case IDENTIFY_ERROR:res = "����ʧ�ܣ���ʶ�����淶��"; break;
	case VAL_EXCESSIVE:res = "����ʧ�ܣ��������࣡"; break;
	case VAL_ERROR:res = "����ʧ�ܣ���ֵ���淶��"; break;
	case PROGRAM_BEGIN:Compile();/*������ڣ�����ʷ�����δ��JmpJud��ֵ��
		����Ĭ��Ϊ0���ͽ�������������,����compile��ȫ�˳��Ժ󣬲���break��ֱ������
		����ִ�У�ִ����default��break�ɣ���Ȼdefault��Ҳûʲôд*/
	default:
		res = "\n==========�ɹ� 1 ����ʧ�� 0 �������� 0 ��==========\n";
		break;
	}
	if (SUCC == JmpJud) {
		printf("============�������Ŀ%s�ı��빤��=============",argv[1]);
		printf("\n%s", res.c_str());
		return 0;
	}
	else if (SOMETHINGWRONG == JmpJud) {
		printf("\n%s", res.c_str());
	}
	else {
		printf("\n==========%s:%s==========\n", argv[1],res.c_str());
		printf("\n=========== ������������%d�� ============\n", Row_Error);
	}
}








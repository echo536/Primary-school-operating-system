#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define INITSTACKSIZE 20
#define STACKSIZEINCREAMENT 10
#define ARRAYMAXSIZE 256
#define MAXBUFFER 128
#define INTERVAL 10
#define NUMMAXBIT 4
#define BITE 0.01
#define MARKING 100.0
#define EPSINON 0.01

typedef char ElemType_i2p;
typedef double ElemType_cRP;
typedef int s_size;
//����׺���ʽתΪ��׺���ʽ������������ʱ��ŷ��ŵ�ջ
typedef struct {
	ElemType_i2p *top;
	ElemType_i2p *base;
	s_size stacksize;
}stack_i2p;
//���ڼ����׺���ʽ
typedef struct {
	ElemType_cRP *top;
	ElemType_cRP *base;
	s_size stacksize;
}stack_cRP;
//��ų�ʼ�������Ŀ�ʹ�
typedef struct {
	char problem[ARRAYMAXSIZE];
	double result;
}problem_result;

void init_i2p(stack_i2p *s);
void push_i2p(stack_i2p *s, char c);
void pop_i2p(stack_i2p *s, char *c);
void inf2pro(char str[], stack_i2p *s);
void init_cRP(stack_cRP *s);
void push_cRP(stack_cRP *s, double c);
void pop_cRP(stack_cRP *s, double *c);
double calcRP(char str[], stack_cRP *s);
problem_result *initsystem(FILE *fproblems, FILE *fscore);
bool estimate_expression(char problem[]);
void create_test(int num, int problem_num[]);
void create_problem_client(char problem[]);
void create_problem_system(char problem[ARRAYMAXSIZE], int problem_num, FILE *fproblems);
void create_problems(FILE *fproblems);
void practice(problem_result problem_result_arr[], FILE *fscore);
void refer_score(FILE *fscore);
bool estimate_positiveint(char option[]);
int menu();
bool estimate_double(char result[]);
void evaluate(int score, int true_num, int false_num, FILE *fscore);
void save_score(int score, int true_num, int false_num, FILE *fscore);
void back();

//����һ��ȫ����������Ŀ����Ҫ��Ԫ�أ�����ϵͳ�Զ�������Ŀ
char expression_element[MAXBUFFER] = { '0','1','2','3','4','5','6','7','8','9','+','-','*','/','(',')','[',']','{','}','\0' };
int n_problem;                              //�������Ŀ����
int n_score;							    //�ɼ���¼�м�¼��

/************************��׺ת��׺����****************************/
//��ʼ��ջ
void init_i2p(stack_i2p *s)
{
	s->base = (ElemType_i2p *)malloc(INITSTACKSIZE * sizeof(ElemType_i2p));
	if (!s->base)
	{
		printf("�����ڴ�ʧ�ܣ�����");
		exit(0);
	}
	s->top = s->base;                 //�տ�ʼջ��ָ��ջ��
	s->stacksize = INITSTACKSIZE;
}

//��ջ
void push_i2p(stack_i2p *s, char c)
{
	//���ջ����׷�ӿռ�
	if ((s->top - s->base) >= s->stacksize)
	{
		s->base = (ElemType_i2p *)realloc(s->base, (s->stacksize + STACKSIZEINCREAMENT) * sizeof(ElemType_i2p));
		if (!s->base)
		{
			printf("�����ڴ�ʧ�ܣ�����");
			exit(1);
		}

		s->top = s->base + s->stacksize;               //ԭ����topָ��ı��ˣ�Ҫ����ָ��
		s->stacksize += STACKSIZEINCREAMENT;
	}
	*s->top = c;									   //�������
	s->top++;
}

//��ջ
void pop_i2p(stack_i2p *s, char *c)
{
	if (s->top == s->base)
	{
		return;
	}
	*c = *--(s->top);                           //��ջ��Ԫ�ص������޸�topָ��
}

//����׺���ʽת��Ϊ��׺���ʽ(ջ)
void inf2pro(char str[], stack_i2p *s)
{
	int i = 0, j = 0;
	char c;
	char temp_str[1024];

	c = str[i++];
	//��������ַ�Ϊ��=��ʱ��ת�����
	while ('=' != c && 0 <= c && 255 >+c)
	{
		//�����򸡵���ֱ�Ӵ��temp_str��
		while (isdigit(c) || '.' == c)
		{
			temp_str[j++] = c;
			c = str[i++];
			if (!isdigit(c) && '.' != c)               //����һ���ַ����������Ҳ���С����ʱ��temp_str��׷��һ���ո��Դ����������ֺ������
			{
				temp_str[j++] = ' ';
			}
		}
		if (')' == c || ']' == c || '}' == c)         //ջ�ڱ�����֮��ƥ��ķ���
		{
			pop_i2p(s, &c);
			while ('(' != c && '[' != c && '{' != c)  //��ջ�������ڵķ���ȫ������
			{
				temp_str[j++] = c;
				temp_str[j++] = ' ';
				pop_i2p(s, &c);
			}
		}
		else if ('+' == c || '-' == c)
		{
			if (s->top == s->base)                    //��ջֱ�ӰѼӼ�����ջ
			{
				push_i2p(s, c);
			}
			else
			{
				//��ջ�ڷ��ŵ������������ţ�ƥ�䵽������ֹͣ������ֱ��ջΪ��
				if ('(' != *(s->top - 1) && '[' != *(s->top - 1) && '{' != *(s->top - 1))
				{
					do
					{
						pop_i2p(s, &temp_str[j++]);
						temp_str[j++] = ' ';
					} while (s->top != s->base && '(' != *(s->top - 1) && '[' != *(s->top - 1) && '{' != *(s->top - 1));
				}
				push_i2p(s, c);
			}
		}
		//ֱ����ջ
		else if ('*' == c || '/' == c || '(' == c || '[' == c || '{' == c)
		{
			push_i2p(s, c);
		}

		if ('=' != c)
		{
			c = str[i++];
		}
	}
	//ȫ��ת����ɺ���ܻ���һЩ���������ջ�У���ջ�е�����ȫ��׷�ӵ�temp_str��
	while (s->top != s->base)
	{
		pop_i2p(s, &temp_str[j++]);
		temp_str[j++] = ' ';
	}

	strncpy(str, temp_str, j);
	str[j] = '\0';
}
/*****************************************************************/

/*************************�����׺���ʽ����**********************/
//��ʼ��ջ
void init_cRP(stack_cRP *s)
{
	s->base = (ElemType_cRP *)malloc(INITSTACKSIZE * sizeof(ElemType_cRP));
	if (!s->base)
	{
		printf("�����ڴ�ʧ�ܣ�����");
		exit(0);
	}
	s->top = s->base;                 //�տ�ʼջ��ָ��ջ��
	s->stacksize = INITSTACKSIZE;
}

//��ջ
void push_cRP(stack_cRP *s, double c)
{
	//���ջ����׷�ӿռ�
	if ((s->top - s->base) >= s->stacksize)
	{
		s->base = (ElemType_cRP *)realloc(s->base, (s->stacksize + STACKSIZEINCREAMENT) * sizeof(ElemType_cRP));
		if (!s->base)
		{
			printf("�����ڴ�ʧ�ܣ�����");
			exit(1);
		}

		s->top = s->base + s->stacksize;               //ԭ����topָ��ı��ˣ�Ҫ����ָ��
		s->stacksize += STACKSIZEINCREAMENT;
	}
	*s->top = c;									   //�������
	s->top++;
}

//��ջ
void pop_cRP(stack_cRP *s, double *c)
{
	if (s->top == s->base)
	{
		return;
	}
	*c = *--(s->top);                           //��ջ��Ԫ�ص������޸�topָ��
}

//�����׺���ʽ
double calcRP(char str[], stack_cRP *s)
{
	int i = 0, j = 0;
	double num1, num2;                        //��Ŵ������������
	char c, temp_str[MAXBUFFER];			  //��ŵ����ַ������ַ�����ʽ��ŵ�����

	c = str[i++];
	while ('\0' != c)
	{
		while ('+' != c && '-' != c && '*' != c && '/' != c)
		{
			temp_str[j++] = c;
			temp_str[j] = '\0';
			c = str[i++];

			//��c���ڿո��������һ�����ˣ���ʱ������ջ������һ��������c
			if (' ' == c)
			{
				push_cRP(s, atof(temp_str));
				j = 0;
				c = str[i++];
			}
		}
		switch (c)
		{
		case '+':
			pop_cRP(s, &num2);
			pop_cRP(s, &num1);
			push_cRP(s, num1 + num2);
			break;
		case '-':
			pop_cRP(s, &num2);
			pop_cRP(s, &num1);
			push_cRP(s, num1 - num2);
			break;
		case '*':
			pop_cRP(s, &num2);
			pop_cRP(s, &num1);
			push_cRP(s, num1 * num2);
			break;
		case '/':
			pop_cRP(s, &num2);
			pop_cRP(s, &num1);
			if (0 != num2)
			{
				push_cRP(s, num1 / num2);
			}
			break;
		default:
			break;
		}
		c = str[(++i)++];
	}
	pop_cRP(s, &num1);
	return num1;
}
/***************************************************************/

//��ʼ��ϵͳ
problem_result *initsystem(FILE *fproblems, FILE *fscore)
{
	int i;
	stack_i2p s_i2p;
	stack_cRP s_RP;
	char problem_RP[ARRAYMAXSIZE];
	double result;
	problem_result *problem_result_arr;

	init_i2p(&s_i2p);
	init_cRP(&s_RP);
	fseek(fproblems, 0, SEEK_SET);
	fscanf(fproblems, "%d", &n_problem);
	problem_result_arr = (problem_result *)malloc(n_problem * sizeof(problem_result));                     //�ڶ��ж���һ������
	for (i = 0; i < n_problem; i++)
	{
		fscanf(fproblems, "%s", problem_RP);
		strcpy(problem_result_arr[i].problem, problem_RP);
		inf2pro(problem_RP, &s_i2p);
		if ('\0' != problem_RP[0])
		{
			result = calcRP(problem_RP, &s_RP);
			if (result > 0)
			{
				result = ((int)(result / BITE + 0.5)) * BITE;                  //���������С�������λ���󱣴� 
				problem_result_arr[i].result = result;
			}
			else if (result < 0)
			{
				result = ((int)(result / BITE - 0.5)) * BITE;                  //���������С�������λ���󱣴� 
				problem_result_arr[i].result = result;
			}
		}
	}
	fseek(fscore, 0, SEEK_SET);
	fscanf(fscore, "%d", &n_score);
	return problem_result_arr;
}

//�жϱ��ʽ�Ƿ���ϱ�׼
bool estimate_expression(char problem[])
{
	int i, count = 1;                                          //count���ڼ������ֵ����λ��
	int symbol1 = 0, symbol2 = 0, symbol3 = 0;                             //�ֱ����ڱ�־��()[]{}��������ĳ���
	int flag = 0, flag1 = 0, flag2 = 0, flag3 = 0;             //��������ֱ�־��������Ӧ�ĵط����������������Ӧ�ı�־��1

															   //��һ���ַ�ֻ�������ֻ��������� 
	if (!isdigit(problem[0]) &&
		'(' != problem[0] &&
		'[' != problem[0] &&
		'{' != problem[0])
	{
		return false;
	}
	//����0123+123=�����
	if ('0' == problem[0])
	{
		return false;
	}
	//�������ţ���־��1����ƥ�䵽��Ӧ���ź�������0
	if ('(' == problem[0])
	{
		flag1 = 1;
	}
	else if ('[' == problem[0])
	{
		flag2 = 1;
	}
	else if ('{' == problem[0])
	{
		flag3 = 1;
	}
	for (i = 1; i < (int)(strlen(problem)); i++)
	{
		if (isdigit(problem[i - 1]) && !isdigit(problem[i]))
		{
			count = 0;
		}
		//�Ȳ�������Ҳ���������������
		if (!isdigit(problem[i]) &&
			'+' != problem[i] &&
			'-' != problem[i] &&
			'*' != problem[i] &&
			'/' != problem[i] &&
			'(' != problem[i] &&
			')' != problem[i] &&
			'[' != problem[i] &&
			']' != problem[i] &&
			'{' != problem[i] &&
			'}' != problem[i] &&
			'=' != problem[i])
		{
			return false;
		}
		//����ǰ������)]}
		else if (isdigit(problem[i]) &&
			(')' == problem[i - 1] ||
				']' == problem[i - 1] ||
				'}' == problem[i - 1]))
		{
			return  false;
		}
		//+-*/
		else if ('+' == problem[i] ||
			'-' == problem[i] ||
			'*' == problem[i] ||
			'/' == problem[i])
		{
			//������������ţ���������������һ�����������1��ʾ��������1����������������(1)+1=
			if (1 == flag1)
			{
				symbol1 = 1;
			}
			if (1 == flag2)
			{
				symbol2 = 1;
			}
			if (1 == flag3)
			{
				symbol3 = 1;
			}
			//flag��1��ʾ��������һ�������������ֻ�����ֺ͵Ⱥţ�����1=
			flag = 1;
			//+-*/ǰ������+-*/([{
			if ('+' == problem[i - 1] ||
				'-' == problem[i - 1] ||
				'*' == problem[i - 1] ||
				'/' == problem[i - 1] ||
				'(' == problem[i - 1] ||
				'[' == problem[i - 1] ||
				'{' == problem[i - 1])
			{
				return false;
			}
		}
		//([{ǰ����������
		else if ('(' == problem[i])
		{
			if (isdigit(problem[i - 1]) ||
				')' == problem[i - 1] ||
				']' == problem[i - 1] ||
				'}' == problem[i - 1] ||
				1 == flag1)                        //����(
			{
				return false;
			}
			flag1 = 1;                           //��ʾƥ�䵽(����ƥ�䵽)����0
		}
		else if ('[' == problem[i])
		{
			if (isdigit(problem[i - 1]) ||
				')' == problem[i - 1] ||
				']' == problem[i - 1] ||
				'}' == problem[i - 1] ||
				1 == flag1 ||                    //����(����δƥ�䵽),�������([
				1 == flag2)						 //����[
			{
				return false;
			}
			flag2 = 1;
		}
		else if ('{' == problem[i])
		{
			if (isdigit(problem[i - 1]) ||
				')' == problem[i - 1] ||
				']' == problem[i - 1] ||
				'}' == problem[i - 1] ||
				1 == flag1 ||					 //({
				1 == flag2 ||					 //[{
				1 == flag3)                      //����{
			{
				return false;
			}
			flag3 = 1;
		}
		//)]}ǰ�������������С����
		else if (')' == problem[i])
		{
			//��(��)֮��û�������������()��(123)
			if (1 != symbol1)
			{
				return false;
			}
			//û��(��)��ǰһ���ַ���������
			if (flag1 != 1 || !isdigit(problem[i - 1]))
			{
				return false;
			}
			//ƥ��)����1
			symbol1 = 0;
			flag1 = 0;
		}
		else if (']' == problem[i])
		{
			if (1 != symbol2)
			{
				return false;
			}
			if ('+' == problem[i - 1] ||
				'-' == problem[i - 1] ||
				'*' == problem[i - 1] ||
				'/' == problem[i - 1] ||
				'[' == problem[i - 1] ||
				'{' == problem[i - 1] ||
				'}' == problem[i - 1] ||
				flag1 == 1 ||                               //�������[(]
				flag2 != 1)                                 //û��[
			{
				return false;
			}
			symbol2 = 0;
			flag2 = 0;
		}
		else if ('}' == problem[i])
		{
			if (1 != symbol3)
			{
				return false;
			}
			if ('+' == problem[i - 1] ||
				'-' == problem[i - 1] ||
				'*' == problem[i - 1] ||
				'/' == problem[i - 1] ||
				'{' == problem[i - 1] ||
				1 == flag1 ||                              //�������{(}
				1 == flag2 ||							   //�������{[}
				0 == flag3)                                //û��{
			{
				return false;
			}
			symbol3 = 0;
			flag3 = 0;
		}
		else if ('=' == problem[i] && (
			'+' == problem[i - 1] ||                        //+=
			'-' == problem[i - 1] ||						//-=
			'*' == problem[i - 1] ||
			'/' == problem[i - 1] ||
			0 == flag ||                                    //1=
			1 == flag1 ||									//1+(=
			1 == flag2 ||
			1 == flag3
			))
		{
			return false;
		}
		else
		{
			//����������λλ0����
			if ('0' == problem[i] && !isdigit(problem[i - 1]))
			{
				return false;
			}
			//�������ֵ�λ��
			if (isdigit(problem[i - 1]))
			{
				count++;
			}
			if (count > NUMMAXBIT)
			{
				return false;
			}
		}
	}
	if ('=' != problem[--i])                                //�������һ���ַ�����=
	{
		return false;
	}
	return true;
}

//�û������������
void create_problem_client(char problem[])
{
	scanf("%s", problem);
	setbuf(stdin, NULL);
	while (false == estimate_expression(problem))
	{
		printf("������󣬱��ʽ��ֻ�ܰ�������'+'��'-'��'*'��'/'��'('��')'��'['��']'��'{'��'}'��'=',������'='������\n���������룺\n");
		scanf("%s", problem);
		setbuf(stdin, NULL);
	}
}

//ϵͳ�Զ��������
void create_problem_system(char problem[ARRAYMAXSIZE], int problem_count, FILE *fproblems)
{
	time_t t;
	int num, i, j;                    //num��ʾҪ���ɵ���Ŀ�еı��ʽ�е��ַ���
	int element_num;

	element_num = strlen(expression_element);
	srand((unsigned)time(&t));
	for (i = 0; i < problem_count; i++)
	{
		do
		{
			num = rand() % 100;
			for (j = 0; j < num; j++)
			{
				problem[j] = expression_element[rand() % (element_num - 1)];
			}
			problem[j] = '=';
			problem[++j] = '\0';
		} while (false == estimate_expression(problem));
		fprintf(fproblems, "%s\n", problem);
		printf("���ɵ�%d����Ŀ:%s\n", i + 1, problem);
	}
}

//���������
void create_problems(FILE *fproblems)
{
	int option, i = 0;
	char problem_count[MAXBUFFER], problem[ARRAYMAXSIZE] = { 0 };

	//��ȡ����е���Ŀ��
	fseek(fproblems, 0, SEEK_SET);
	fscanf(fproblems, "%d", &n_problem);
	printf("******************��ӭ����Сѧ��������ҵϵͳ*******************\n");
	printf("\t\t\t+------------+\n"
		"\t\t\t|    Ŀ¼    |\n"
		"\t\t\t+------------+\n"
		"\t\t\t| 1.�������� |\n"
		"\t\t\t+------------+\n"
		"\t\t\t| 2.ϵͳ���� |\n"
		"\t\t\t+------------+\n");
	if (0 == n_problem)
	{
		printf("������л�û����⣬�뽨������⣺\n");
	}
	printf("��ѡ�������");

	scanf("%d", &option);
	setbuf(stdin, NULL);
	while (1 != option && 2 != option)
	{
		printf("����������������룺");
		scanf("%d", &option);
		setbuf(stdin, NULL);
	}

	system("cls");
	printf("******************��ӭ����Сѧ��������ҵϵͳ*******************\n\n");
	printf("��������Ҫ������Ŀ��������");
	scanf("%s", problem_count);
	setbuf(stdin, NULL);
	while (false == estimate_positiveint(problem_count))
	{
		printf("����������������루����Ϊ����������");
		scanf("%s", problem_count);
		setbuf(stdin, NULL);
	}
	putchar('\n');
	n_problem += atoi(problem_count);
	fseek(fproblems, 0, SEEK_SET);
	fprintf(fproblems, "%d", n_problem);
	fseek(fproblems, 0, SEEK_END);                        //���ļ�λ��ָ��ָ���ļ�ĩβ����ֹд��ʱ������ԭ��������
	if (1 == option)
	{
		printf("��ʾ�� ���ʽ��ֻ�ܰ������ּ����·��ţ�\n"
			"       '+'��'-'��'*'��'/'��'('��')'��'['��']'��'{'��'}'��'='\n"
			"       ������'='����\n"
			"       ���磺 1+2-{[(3-4)*5]/6}+7=\n\n");
		for (i = 0; i < atoi(problem_count); i++)
		{
			printf("�������%d����Ŀ��\n", i + 1);
			create_problem_client(problem);
			fprintf(fproblems, "%s\n", problem);
		}
	}
	else
	{
		create_problem_system(problem, atoi(problem_count), fproblems);
		putchar('\n');
	}

	fflush(fproblems);
	printf("����Ѹ��£�����������������˵�");
	getchar();
	setbuf(stdin, NULL);
}

//�ж������ǲ���һ�������� 
bool estimate_double(char result[])
{
	int i = 0;

	if (!isdigit(result[i]) && '.' != result[i] && '-' != result[i++])
	{
		return false;
	}
	if ('.' == result[0])
	{
		return false;
	}
	while (!isdigit(result[i]) && '.' != result[i++])
	{
		return false;
	}
	return true;
}

//����
void back()
{
	system("pause");
	setbuf(stdin, NULL);
}

//�ɼ�����
void evaluate(int score, int true_num, int false_num, FILE *fscore)
{
	char blank[11] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0' };
	int temp[4];
	int i;
	char c;
	char detail_log[ARRAYMAXSIZE];
	int true_per;

	temp[0] = score;
	temp[1] = (int)(100 * true_num / (true_num + false_num));
	temp[2] = true_num;
	temp[3] = false_num;
	system("cls");
	printf("******************��ӭ����Сѧ��������ҵϵͳ*******************\n");
	printf("   +----------+----------+----------+----------+----------+\n"
		"   |   ����   |   �÷�   |  ��ȷ��  | ��ȷ���� | �������� |\n"
		"   +----------+----------+----------+----------+----------+\n");
	printf("   |   ��ǰ   |");
	for (i = 0; i < 4; i++)
	{
		sprintf(detail_log, "%d", temp[i]);
		if (1 == i)
		{
			strcat(detail_log, "%");                               //��ȷ�ʺ�Ӱٷֺ�
		}
		printf("%.*s", (INTERVAL - strlen(detail_log)) / 2, blank);
		printf("%s", detail_log);
		printf("%.*s|", ((INTERVAL - strlen(detail_log)) / 2) + ((1 == (strlen(detail_log) % 2)) ? 1 : 0), blank);
	}
	printf("\n   |");
	//�ɼ��������м�¼
	if (0 != n_score)
	{
		fseek(fscore, -1, SEEK_END);
		do
		{
			fseek(fscore, -2, SEEK_CUR);
			c = fgetc(fscore);
		} while (c != '\n');
	}

	for (i = 0; i < 5; i++)
	{
		//���0
		if (0 == n_score)
		{
			detail_log[0] = '0';
			detail_log[1] = '\0';
		}
		else
		{
			fscanf(fscore, "%s", detail_log);
		}
		if (2 == i)
		{
			true_per = atoi(detail_log);
			strcat(detail_log, "%");
		}
		printf("%.*s", (INTERVAL - strlen(detail_log)) / 2, blank);
		printf("%s", detail_log);
		printf("%.*s|", ((INTERVAL - strlen(detail_log)) / 2) + ((1 == (strlen(detail_log) % 2)) ? 1 : 0), blank);
	}
	printf("\n   +----------+----------+----------+----------+----------+\n\n");

	if (temp[1] > true_per)
	{
		printf("�����ˣ�\n");
	}
	else if (temp[1] == true_per)
	{
		printf("��ȷ�ʸ��ϴ�һ����\n");
	}
	else
	{
		printf("�˲��ˣ�\n");
	}
}

//�������num����Ŀ
void create_test(int num, int problem_num[])
{
	time_t t;
	int i, j;

	srand((unsigned)time(&t));
	for (i = 0; i < num; i++)
	{
		problem_num[i] = rand() % n_problem;
		j = 0;
		while (j < i)
		{
			while (problem_num[i] == problem_num[j])
			{
				problem_num[i] = rand() % num;
				j = 0;                                      //����ֵ�����¿�ʼ�Ƚ�
			}
			j++;
		}

	}
}

//����
void practice(problem_result problem_result_arr[], FILE *fscore)
{
	int i, true_num = 0, false_num = 0;      //�ɼ�����ȷ��������������
	char num[MAXBUFFER];                        //�û���Ҫ��ϰ����Ŀ�� 
	int *problem_num;				//������ɵ���Ŀ�� 
	char option;
	char result[MAXBUFFER];
	double score = 0;

	problem_num = (int *)malloc(n_problem * sizeof(int));                      //�ڶ��ж���һ������
	printf("******************��ӭ����Сѧ��������ҵϵͳ*******************\n\n");
	printf("��������Ҫ��ϰ����Ŀ��(������)��");
	scanf("%s", num);
	setbuf(stdin, NULL);
	//num��������������num����������num������Ŀ��
	while (0 == atoi(num) || false == estimate_positiveint(num) || (true == estimate_positiveint(num) && n_problem < atoi(num)))
	{
		if (n_problem < atoi(num))
		{
			printf("�����ֻ��%d���⣬���������룺", n_problem);
		}
		else
		{
			printf("�����������������һ����������");
		}
		scanf("%s", num);
		setbuf(stdin, NULL);
	}

	create_test(atoi(num), problem_num);

	printf("\n��Ŀ�����ɣ�\n");
	//����������Ŀ����ӡ���� 
	for (i = 0; i < atoi(num); i++)
	{
		printf("%s\t", problem_result_arr[problem_num[i]].problem);

		//ÿ5��һ�� 
		if (4 == i % 10 || 9 == i % 10)
		{
			putchar('\n');
		}
	}
	if ((i - 1) < 4 || 0 != (i - 1) % 4)
	{
		putchar('\n');
	}

	printf("\n���ڿ�ʼ������Y/N��:");
	scanf("%c", &option);
	setbuf(stdin, NULL);
	while ('Y' != option && 'y' != option && 'N' != option && 'n' != option)
	{
		printf("����������������루Y/N��:");
		scanf("%c", &option);
		setbuf(stdin, NULL);
	}
	if ('Y' == option || 'y' == option)
	{
		system("cls");
		printf("******************��ӭ����Сѧ��������ҵϵͳ*******************\n\n");
		printf("��ʾ:С���������λ��Ч���֣�\n\n���ڿ�ʼ���⣺\n");
		for (i = 0; i < atoi(num); i++)
		{
			printf("��%d��:\n", i + 1);
			printf("%s", problem_result_arr[problem_num[i]].problem);
			scanf("%s", result);
			setbuf(stdin, NULL);
			while (false == estimate_double(result))
			{
				printf("������󣨴���ֻ�ܰ������ֺ�С���㣩\n���������룺");
				scanf("%s", result);
				setbuf(stdin, NULL);
			}
			//�������⣬����������ֱ����==�����ж� 
			if (atof(result) - problem_result_arr[problem_num[i]].result <= EPSINON &&
				atof(result) - problem_result_arr[problem_num[i]].result >= -EPSINON)
			{
				score++;
				true_num++;
				printf("\n�ش���ȷ����%d��\nĿǰ�÷֣�%d\n\n", (int)(MARKING / atoi(num)), (int)(score / atoi(num) * MARKING));
			}
			else
			{
				false_num++;
				printf("\n�ش����,��ȷ����%.2f\nĿǰ�÷֣�%d��\n\n", problem_result_arr[problem_num[i]].result, (int)(score / atoi(num) * 100));
			}
		}
		printf("\n������Ŀ�Ѵ���,����������鿴����");
		getchar();
		setbuf(stdin, NULL);

		evaluate((int)(score / atoi(num) * 100), true_num, false_num, fscore);
		save_score((int)(score / atoi(num) * 100), true_num, false_num, fscore);
	}
	back();
}

//���������¼
void save_score(int score, int true_num, int false_num, FILE *fscore)
{
	time_t t;
	tm *history_practice_time;

	t = time(NULL);
	history_practice_time = localtime(&t);
	//��ȡ��ǰ�ķ�����¼������һ
	fseek(fscore, 0, SEEK_SET);
	fscanf(fscore, "%d", &n_score);
	fseek(fscore, 0, SEEK_SET);
	fprintf(fscore, "%d", ++n_score);
	//д�����ںͳɼ�
	fseek(fscore, 0, SEEK_END);
	fprintf(fscore, "%d/%d/%d %d %d %d %d\n",
		1900 + history_practice_time->tm_year,                //��
		history_practice_time->tm_mon + 1,					  //��
		history_practice_time->tm_mday,						  //��
		score,												  //����
		(int)((float)true_num / (true_num + false_num) * 100),     //��ȷ��
		true_num,                                             //��ȷ����
		false_num);											  //��������
}

//��ѯ�ɼ�
void refer_score(FILE *fscore)
{
	int num_log, i, j;
	char detail_log[ARRAYMAXSIZE];
	char blank[11] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0' };        //���ھ�����ʾ
	float true_num = 0, false_num = 0;
	float average[4];

	fseek(fscore, 0, SEEK_SET);
	fscanf(fscore, "%d", &num_log);                          //��ȡ��¼��
	printf("******************��ӭ����Сѧ��������ҵϵͳ*******************\n\n");
	printf("   ��־��һ����%d����¼��\n", num_log);
	printf("   +----------+----------+----------+----------+----------+\n"
		"   |   ����   |   �÷�   |  ��ȷ��  | ��ȷ���� | �������� |\n"
		"   +----------+----------+----------+----------+----------+\n");
	for (i = 0; i < num_log; i++)
	{
		printf("   |");
		//��ӡ��
		for (j = 0; j < 5; j++)
		{
			fscanf(fscore, "%s", &detail_log);
			if (2 == j)
			{
				strcat(detail_log, "%");                               //��ȷ�ʺ�Ӱٷֺ�
			}
			//������ʾ
			//�����Ա�ĳ��ȼ�ȥ��ǰ��Ա�ĳ���
			//����2�ٷֱ��ӡ����
			printf("%.*s", (INTERVAL - strlen(detail_log)) / 2, blank);
			printf("%s", detail_log);
			printf("%.*s|", ((INTERVAL - strlen(detail_log)) / 2) + ((1 == (strlen(detail_log) % 2)) ? 1 : 0), blank);     //����Ա������ż�������ұ߿ո����������߶�һ

																														   //����true_num���ۼ�ֵ
			if (3 == j % 10 || 8 == j % 10)
			{
				true_num += (float)atof(detail_log);
			}

			//����true_num���ۼ�ֵ
			if (4 == j % 10 || 9 == j % 10)
			{
				false_num += (float)atof(detail_log);
			}
		}
		putchar('\n');

	}

	//����ƽ��ֵ��������
	if (0 == num_log)
	{
		average[0] = 0;
		average[1] = 0;
		average[2] = 0;
		average[3] = 0;
	}
	else
	{
		average[0] = true_num / n_score;
		average[1] = 100 * true_num / (true_num + false_num);
		average[2] = true_num / n_score;
		average[3] = false_num / n_score;
	}
	printf("   |  ƽ����  |");
	for (i = 0; i < 4; i++)
	{
		sprintf(detail_log, "%.2f", average[i]);                   //ת��Ϊ�ַ������ܾ�����ʾ
		if (1 == i)
		{
			strcat(detail_log, "%");                               //��ȷ�ʺ�Ӱٷֺ�
		}
		printf("%.*s", (INTERVAL - strlen(detail_log)) / 2, blank);
		printf("%s", detail_log);
		printf("%.*s|", ((INTERVAL - strlen(detail_log)) / 2) + ((1 == (strlen(detail_log) % 2)) ? 1 : 0), blank);     //����Ա������ż�������ұ߿ո����������߶�һ
	}
	printf("\n   +----------+----------+----------+----------+----------+\n\n");
}

//�ж��Ƿ�Ϊ���������Ƿ���true�����򷵻�false
bool estimate_positiveint(char str[])
{
	int i = 0;

	while ('\0' != str[i])
	{
		if (!isdigit(str[i++]))         //���ַ�������һ���ַ��������֣��򷵻�false
		{
			return false;
		}
	}
	return true;
}

//������ʽ��� 
void solution()
{
	stack_i2p s_i2p;
	stack_cRP s_RP;
	char problem[ARRAYMAXSIZE];

	init_i2p(&s_i2p);
	init_cRP(&s_RP);
	printf("******************��ӭ����Сѧ��������ҵϵͳ*******************\n\n");
	printf("��ʾ��(1) �����˳���������Q!\n"
		"      (2)���ʽ��ֻ�ܰ������ּ����·��ţ�\n"
		"         '+'��'-'��'*'��'/'��'('��')'��'['��']'��'{'��'}'��'=''������' = '����\n"
		"         ���磺1+2-{[(3-4)*5]/6}+7=\n\n"
		"���ڿ�ʼ���룺\n\n");
	while (true)
	{
		scanf("%s", problem);
		setbuf(stdin, NULL);
		while (false == estimate_expression(problem))
		{
			if ('q' == problem[0] || 'Q' == problem[0])
			{
				return;
			}
			printf("������󣬱��ʽ��ֻ�ܰ�������'+'��'-'��'*'��'/'��'('��')'��'['��']'��'{'��'}'��'=',������'='������\n���������룺\n");
			scanf("%s", problem);
			setbuf(stdin, NULL);
		}
		inf2pro(problem, &s_i2p);
		if ('\0' != problem[0])
		{
			printf("%.2f\n", calcRP(problem, &s_RP));
		}
	}
}

//����Ŀ¼
int menu()
{
	int option;

	system("cls");
	printf("******************��ӭ����Сѧ��������ҵϵͳ*******************\n");
	printf("\t\t+----------------------------+\n"
		"\t\t|            Ŀ¼            |\n"
		"\t\t+----------------------------+\n"
		"\t\t|        1.������          |\n"
		"\t\t+----------------------------+\n"
		"\t\t|        2.��ϰ              |\n"
		"\t\t+----------------------------+\n"
		"\t\t|        3.�鿴��ʷ�ɼ�      |\n"
		"\t\t+----------------------------+\n"
		"\t\t|        4.���            |\n"
		"\t\t+----------------------------+\n"
		"\t\t|        5.�˳�ϵͳ          |\n"
		"\t\t+----------------------------+\n\n"
		"��ѡ�������");
	scanf("%d", &option);
	setbuf(stdin, NULL);
	while (1 != option && 2 != option && 3 != option && 4 != option && 5 != option)
	{
		printf("����������������루����������");
		scanf("%d", &option);
		setbuf(stdin, NULL);
	}
	return option;
}

int main(void)
{
	FILE *fproblems;
	FILE *fscore;
	problem_result *problem_result_arr = NULL;
	int option;

	fproblems = fopen("problems.txt", "r+");
	fscore = fopen("score.txt", "r+");
	option = menu();
	if (NULL == fproblems && option != 5)
	{
		system("cls");
		fproblems = fopen("problems.txt", "a");
		fclose(fproblems);
		fproblems = fopen("problems.txt", "r+");
		fprintf(fproblems, "%d          \n", 0);                     //�տ�ʼ����е���Ŀ��Ϊ0
		fflush(fproblems);
		create_problems(fproblems);
		option = 0;
	}
	if (NULL == fscore && option != 5)
	{
		fscore = fopen("score.txt", "a");
		fclose(fscore);
		fscore = fopen("score.txt", "r+");
		fprintf(fscore, "%d          \n", 0);                     //�տ�ʼ��0���ɼ���¼
		fflush(fscore);
	}
	do
	{
		if (5 != option)
		{
			problem_result_arr = initsystem(fproblems, fscore);
			system("cls");
		}
		switch (option)
		{
		case 1:
			create_problems(fproblems);
			system("cls");
			break;
		case 2:
			if (0 != n_problem)
			{
				practice(problem_result_arr, fscore);
			}
			else
			{
				create_problems(fproblems);
			}
			break;
		case 3:
			refer_score(fscore);
			back();
			break;
		case 4:
			solution();
			break;
		case 5:
			exit(1);
			break;
		default:
			break;
		}
	} while (option = menu());

	fclose(fproblems);
	fclose(fscore);

	return 0;
}

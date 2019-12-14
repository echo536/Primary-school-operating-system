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
//在中缀表达式转为后缀表达式过程中用来临时存放符号的栈
typedef struct {
	ElemType_i2p *top;
	ElemType_i2p *base;
	s_size stacksize;
}stack_i2p;
//用于计算后缀表达式
typedef struct {
	ElemType_cRP *top;
	ElemType_cRP *base;
	s_size stacksize;
}stack_cRP;
//存放初始化后的题目和答案
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

//定义一个全局数组存放题目所需要的元素，用来系统自动生成题目
char expression_element[MAXBUFFER] = { '0','1','2','3','4','5','6','7','8','9','+','-','*','/','(',')','[',']','{','}','\0' };
int n_problem;                              //题库中题目数量
int n_score;							    //成绩记录中记录数

/************************中缀转后缀操作****************************/
//初始化栈
void init_i2p(stack_i2p *s)
{
	s->base = (ElemType_i2p *)malloc(INITSTACKSIZE * sizeof(ElemType_i2p));
	if (!s->base)
	{
		printf("申请内存失败！！！");
		exit(0);
	}
	s->top = s->base;                 //刚开始栈顶指向栈底
	s->stacksize = INITSTACKSIZE;
}

//入栈
void push_i2p(stack_i2p *s, char c)
{
	//如果栈满，追加空间
	if ((s->top - s->base) >= s->stacksize)
	{
		s->base = (ElemType_i2p *)realloc(s->base, (s->stacksize + STACKSIZEINCREAMENT) * sizeof(ElemType_i2p));
		if (!s->base)
		{
			printf("申请内存失败！！！");
			exit(1);
		}

		s->top = s->base + s->stacksize;               //原来的top指向改变了，要重新指向
		s->stacksize += STACKSIZEINCREAMENT;
	}
	*s->top = c;									   //存放数据
	s->top++;
}

//出栈
void pop_i2p(stack_i2p *s, char *c)
{
	if (s->top == s->base)
	{
		return;
	}
	*c = *--(s->top);                           //将栈顶元素弹出并修改top指针
}

//将中缀表达式转换为后缀表达式(栈)
void inf2pro(char str[], stack_i2p *s)
{
	int i = 0, j = 0;
	char c;
	char temp_str[1024];

	c = str[i++];
	//当输入的字符为‘=’时即转换完成
	while ('=' != c && 0 <= c && 255 >+c)
	{
		//整数或浮点数直接存进temp_str中
		while (isdigit(c) || '.' == c)
		{
			temp_str[j++] = c;
			c = str[i++];
			if (!isdigit(c) && '.' != c)               //当下一个字符不是数字且不是小数点时在temp_str中追加一个空格，以此来区分数字和运算符
			{
				temp_str[j++] = ' ';
			}
		}
		if (')' == c || ']' == c || '}' == c)         //栈内必有与之相匹配的符号
		{
			pop_i2p(s, &c);
			while ('(' != c && '[' != c && '{' != c)  //把栈中括号内的符号全部弹出
			{
				temp_str[j++] = c;
				temp_str[j++] = ' ';
				pop_i2p(s, &c);
			}
		}
		else if ('+' == c || '-' == c)
		{
			if (s->top == s->base)                    //空栈直接把加减号入栈
			{
				push_i2p(s, c);
			}
			else
			{
				//把栈内符号弹出。若有括号，匹配到括号则停止，否则直到栈为空
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
		//直接入栈
		else if ('*' == c || '/' == c || '(' == c || '[' == c || '{' == c)
		{
			push_i2p(s, c);
		}

		if ('=' != c)
		{
			c = str[i++];
		}
	}
	//全部转换完成后可能还有一些运算符号在栈中，将栈中的运算全部追加到temp_str中
	while (s->top != s->base)
	{
		pop_i2p(s, &temp_str[j++]);
		temp_str[j++] = ' ';
	}

	strncpy(str, temp_str, j);
	str[j] = '\0';
}
/*****************************************************************/

/*************************计算后缀表达式操作**********************/
//初始化栈
void init_cRP(stack_cRP *s)
{
	s->base = (ElemType_cRP *)malloc(INITSTACKSIZE * sizeof(ElemType_cRP));
	if (!s->base)
	{
		printf("申请内存失败！！！");
		exit(0);
	}
	s->top = s->base;                 //刚开始栈顶指向栈底
	s->stacksize = INITSTACKSIZE;
}

//入栈
void push_cRP(stack_cRP *s, double c)
{
	//如果栈满，追加空间
	if ((s->top - s->base) >= s->stacksize)
	{
		s->base = (ElemType_cRP *)realloc(s->base, (s->stacksize + STACKSIZEINCREAMENT) * sizeof(ElemType_cRP));
		if (!s->base)
		{
			printf("申请内存失败！！！");
			exit(1);
		}

		s->top = s->base + s->stacksize;               //原来的top指向改变了，要重新指向
		s->stacksize += STACKSIZEINCREAMENT;
	}
	*s->top = c;									   //存放数据
	s->top++;
}

//出栈
void pop_cRP(stack_cRP *s, double *c)
{
	if (s->top == s->base)
	{
		return;
	}
	*c = *--(s->top);                           //将栈顶元素弹出并修改top指针
}

//计算后缀表达式
double calcRP(char str[], stack_cRP *s)
{
	int i = 0, j = 0;
	double num1, num2;                        //存放待运算的两个数
	char c, temp_str[MAXBUFFER];			  //存放单个字符和以字符串形式存放的数字

	c = str[i++];
	while ('\0' != c)
	{
		while ('+' != c && '-' != c && '*' != c && '/' != c)
		{
			temp_str[j++] = c;
			temp_str[j] = '\0';
			c = str[i++];

			//若c等于空格，则接受完一个数了，这时将其入栈并把下一个数赋给c
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

//初始化系统
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
	problem_result_arr = (problem_result *)malloc(n_problem * sizeof(problem_result));                     //在堆中定义一个数组
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
				result = ((int)(result / BITE + 0.5)) * BITE;                  //将结果保留小数点后两位数后保存 
				problem_result_arr[i].result = result;
			}
			else if (result < 0)
			{
				result = ((int)(result / BITE - 0.5)) * BITE;                  //将结果保留小数点后两位数后保存 
				problem_result_arr[i].result = result;
			}
		}
	}
	fseek(fscore, 0, SEEK_SET);
	fscanf(fscore, "%d", &n_score);
	return problem_result_arr;
}

//判断表达式是否符合标准
bool estimate_expression(char problem[])
{
	int i, count = 1;                                          //count用于计算数字的最大位数
	int symbol1 = 0, symbol2 = 0, symbol3 = 0;                             //分别用于标志在()[]{}内运算符的出现
	int flag = 0, flag1 = 0, flag2 = 0, flag3 = 0;             //运算符出现标志，若在相应的地方出现运算符，则相应的标志置1

															   //第一个字符只能是数字或者左括号 
	if (!isdigit(problem[0]) &&
		'(' != problem[0] &&
		'[' != problem[0] &&
		'{' != problem[0])
	{
		return false;
	}
	//出现0123+123=的情况
	if ('0' == problem[0])
	{
		return false;
	}
	//若有括号，标志置1，待匹配到对应括号后重新置0
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
		//既不是数字也不是运算符和括号
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
		//数字前不能有)]}
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
			//若左边已有括号，则括号内至少有一个运算符，置1表示已有至少1个运算符，避免出现(1)+1=
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
			//flag置1表示已有至少一个运算符，避免只有数字和等号，出现1=
			flag = 1;
			//+-*/前不能是+-*/([{
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
		//([{前不能有数字
		else if ('(' == problem[i])
		{
			if (isdigit(problem[i - 1]) ||
				')' == problem[i - 1] ||
				']' == problem[i - 1] ||
				'}' == problem[i - 1] ||
				1 == flag1)                        //已有(
			{
				return false;
			}
			flag1 = 1;                           //表示匹配到(，待匹配到)后置0
		}
		else if ('[' == problem[i])
		{
			if (isdigit(problem[i - 1]) ||
				')' == problem[i - 1] ||
				']' == problem[i - 1] ||
				'}' == problem[i - 1] ||
				1 == flag1 ||                    //已有(但还未匹配到),避免出现([
				1 == flag2)						 //已有[
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
				1 == flag3)                      //已有{
			{
				return false;
			}
			flag3 = 1;
		}
		//)]}前不能有运算符和小括号
		else if (')' == problem[i])
		{
			//从(到)之间没有运算符，避免()或(123)
			if (1 != symbol1)
			{
				return false;
			}
			//没有(或)的前一个字符不是数字
			if (flag1 != 1 || !isdigit(problem[i - 1]))
			{
				return false;
			}
			//匹配)后置1
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
				flag1 == 1 ||                               //避免出现[(]
				flag2 != 1)                                 //没有[
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
				1 == flag1 ||                              //避免出现{(}
				1 == flag2 ||							   //避免出现{[}
				0 == flag3)                                //没有{
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
			//避免出现最高位位0的数
			if ('0' == problem[i] && !isdigit(problem[i - 1]))
			{
				return false;
			}
			//计算数字的位数
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
	if ('=' != problem[--i])                                //避免最后一个字符不是=
	{
		return false;
	}
	return true;
}

//用户自行输入题库
void create_problem_client(char problem[])
{
	scanf("%s", problem);
	setbuf(stdin, NULL);
	while (false == estimate_expression(problem))
	{
		printf("输入错误，表达式中只能包含数字'+'、'-'、'*'、'/'、'('、')'、'['、']'、'{'、'}'、'=',并且以'='结束！\n请重新输入：\n");
		scanf("%s", problem);
		setbuf(stdin, NULL);
	}
}

//系统自动生成题库
void create_problem_system(char problem[ARRAYMAXSIZE], int problem_count, FILE *fproblems)
{
	time_t t;
	int num, i, j;                    //num表示要生成的题目中的表达式中的字符数
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
		printf("生成第%d个题目:%s\n", i + 1, problem);
	}
}

//建立试题库
void create_problems(FILE *fproblems)
{
	int option, i = 0;
	char problem_count[MAXBUFFER], problem[ARRAYMAXSIZE] = { 0 };

	//读取题库中的题目数
	fseek(fproblems, 0, SEEK_SET);
	fscanf(fproblems, "%d", &n_problem);
	printf("******************欢迎来到小学生算术作业系统*******************\n");
	printf("\t\t\t+------------+\n"
		"\t\t\t|    目录    |\n"
		"\t\t\t+------------+\n"
		"\t\t\t| 1.自行生成 |\n"
		"\t\t\t+------------+\n"
		"\t\t\t| 2.系统生成 |\n"
		"\t\t\t+------------+\n");
	if (0 == n_problem)
	{
		printf("试题库中还没有题库，请建立试题库：\n");
	}
	printf("请选择操作：");

	scanf("%d", &option);
	setbuf(stdin, NULL);
	while (1 != option && 2 != option)
	{
		printf("输入错误，请重新输入：");
		scanf("%d", &option);
		setbuf(stdin, NULL);
	}

	system("cls");
	printf("******************欢迎来到小学生算术作业系统*******************\n\n");
	printf("请输入需要生成题目的数量：");
	scanf("%s", problem_count);
	setbuf(stdin, NULL);
	while (false == estimate_positiveint(problem_count))
	{
		printf("输入错误，请重新输入（必须为正整数）：");
		scanf("%s", problem_count);
		setbuf(stdin, NULL);
	}
	putchar('\n');
	n_problem += atoi(problem_count);
	fseek(fproblems, 0, SEEK_SET);
	fprintf(fproblems, "%d", n_problem);
	fseek(fproblems, 0, SEEK_END);                        //将文件位置指针指向文件末尾，防止写入时覆盖了原来的内容
	if (1 == option)
	{
		printf("提示： 表达式中只能包含数字及以下符号：\n"
			"       '+'、'-'、'*'、'/'、'('、')'、'['、']'、'{'、'}'、'='\n"
			"       并且以'='结束\n"
			"       例如： 1+2-{[(3-4)*5]/6}+7=\n\n");
		for (i = 0; i < atoi(problem_count); i++)
		{
			printf("请输入第%d个题目：\n", i + 1);
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
	printf("题库已更新，输入任意键返回主菜单");
	getchar();
	setbuf(stdin, NULL);
}

//判断输入是不是一个浮点数 
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

//返回
void back()
{
	system("pause");
	setbuf(stdin, NULL);
}

//成绩评价
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
	printf("******************欢迎来到小学生算术作业系统*******************\n");
	printf("   +----------+----------+----------+----------+----------+\n"
		"   |   日期   |   得分   |  正确率  | 正确题数 | 错误题数 |\n"
		"   +----------+----------+----------+----------+----------+\n");
	printf("   |   当前   |");
	for (i = 0; i < 4; i++)
	{
		sprintf(detail_log, "%d", temp[i]);
		if (1 == i)
		{
			strcat(detail_log, "%");                               //正确率后加百分号
		}
		printf("%.*s", (INTERVAL - strlen(detail_log)) / 2, blank);
		printf("%s", detail_log);
		printf("%.*s|", ((INTERVAL - strlen(detail_log)) / 2) + ((1 == (strlen(detail_log) % 2)) ? 1 : 0), blank);
	}
	printf("\n   |");
	//成绩表中已有记录
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
		//输出0
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
		printf("进步了！\n");
	}
	else if (temp[1] == true_per)
	{
		printf("正确率跟上次一样！\n");
	}
	else
	{
		printf("退步了！\n");
	}
}

//随机生成num个题目
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
				j = 0;                                      //赋新值后重新开始比较
			}
			j++;
		}

	}
}

//做题
void practice(problem_result problem_result_arr[], FILE *fscore)
{
	int i, true_num = 0, false_num = 0;      //成绩，正确题数，错误题数
	char num[MAXBUFFER];                        //用户需要练习的题目数 
	int *problem_num;				//随机生成的题目号 
	char option;
	char result[MAXBUFFER];
	double score = 0;

	problem_num = (int *)malloc(n_problem * sizeof(int));                      //在堆中定义一个数组
	printf("******************欢迎来到小学生算术作业系统*******************\n\n");
	printf("请输入你要练习的题目数(正整数)：");
	scanf("%s", num);
	setbuf(stdin, NULL);
	//num不是正整数或者num是正整数但num大于题目数
	while (0 == atoi(num) || false == estimate_positiveint(num) || (true == estimate_positiveint(num) && n_problem < atoi(num)))
	{
		if (n_problem < atoi(num))
		{
			printf("题库中只有%d道题，请重新输入：", n_problem);
		}
		else
		{
			printf("输入错误，请重新输入一个正整数：");
		}
		scanf("%s", num);
		setbuf(stdin, NULL);
	}

	create_test(atoi(num), problem_num);

	printf("\n题目已生成：\n");
	//遍历所有题目并打印出来 
	for (i = 0; i < atoi(num); i++)
	{
		printf("%s\t", problem_result_arr[problem_num[i]].problem);

		//每5个一行 
		if (4 == i % 10 || 9 == i % 10)
		{
			putchar('\n');
		}
	}
	if ((i - 1) < 4 || 0 != (i - 1) % 4)
	{
		putchar('\n');
	}

	printf("\n现在开始做题吗（Y/N）:");
	scanf("%c", &option);
	setbuf(stdin, NULL);
	while ('Y' != option && 'y' != option && 'N' != option && 'n' != option)
	{
		printf("输入错误，请重新输入（Y/N）:");
		scanf("%c", &option);
		setbuf(stdin, NULL);
	}
	if ('Y' == option || 'y' == option)
	{
		system("cls");
		printf("******************欢迎来到小学生算术作业系统*******************\n\n");
		printf("提示:小数点后保留两位有效数字！\n\n现在开始答题：\n");
		for (i = 0; i < atoi(num); i++)
		{
			printf("第%d题:\n", i + 1);
			printf("%s", problem_result_arr[problem_num[i]].problem);
			scanf("%s", result);
			setbuf(stdin, NULL);
			while (false == estimate_double(result))
			{
				printf("输入错误（答案中只能包括数字和小数点）\n请重新输入：");
				scanf("%s", result);
				setbuf(stdin, NULL);
			}
			//精度问题，浮点数不能直接用==进行判断 
			if (atof(result) - problem_result_arr[problem_num[i]].result <= EPSINON &&
				atof(result) - problem_result_arr[problem_num[i]].result >= -EPSINON)
			{
				score++;
				true_num++;
				printf("\n回答正确，加%d分\n目前得分：%d\n\n", (int)(MARKING / atoi(num)), (int)(score / atoi(num) * MARKING));
			}
			else
			{
				false_num++;
				printf("\n回答错误,正确答案是%.2f\n目前得分：%d分\n\n", problem_result_arr[problem_num[i]].result, (int)(score / atoi(num) * 100));
			}
		}
		printf("\n所有题目已答完,输入任意键查看评价");
		getchar();
		setbuf(stdin, NULL);

		evaluate((int)(score / atoi(num) * 100), true_num, false_num, fscore);
		save_score((int)(score / atoi(num) * 100), true_num, false_num, fscore);
	}
	back();
}

//保存分数记录
void save_score(int score, int true_num, int false_num, FILE *fscore)
{
	time_t t;
	tm *history_practice_time;

	t = time(NULL);
	history_practice_time = localtime(&t);
	//获取当前的分数记录数并加一
	fseek(fscore, 0, SEEK_SET);
	fscanf(fscore, "%d", &n_score);
	fseek(fscore, 0, SEEK_SET);
	fprintf(fscore, "%d", ++n_score);
	//写入日期和成绩
	fseek(fscore, 0, SEEK_END);
	fprintf(fscore, "%d/%d/%d %d %d %d %d\n",
		1900 + history_practice_time->tm_year,                //年
		history_practice_time->tm_mon + 1,					  //月
		history_practice_time->tm_mday,						  //日
		score,												  //分数
		(int)((float)true_num / (true_num + false_num) * 100),     //正确率
		true_num,                                             //正确题数
		false_num);											  //错误题数
}

//查询成绩
void refer_score(FILE *fscore)
{
	int num_log, i, j;
	char detail_log[ARRAYMAXSIZE];
	char blank[11] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\0' };        //用于居中显示
	float true_num = 0, false_num = 0;
	float average[4];

	fseek(fscore, 0, SEEK_SET);
	fscanf(fscore, "%d", &num_log);                          //读取记录数
	printf("******************欢迎来到小学生算术作业系统*******************\n\n");
	printf("   日志中一共有%d条记录：\n", num_log);
	printf("   +----------+----------+----------+----------+----------+\n"
		"   |   日期   |   得分   |  正确率  | 正确题数 | 错误题数 |\n"
		"   +----------+----------+----------+----------+----------+\n");
	for (i = 0; i < num_log; i++)
	{
		printf("   |");
		//打印行
		for (j = 0; j < 5; j++)
		{
			fscanf(fscore, "%s", &detail_log);
			if (2 == j)
			{
				strcat(detail_log, "%");                               //正确率后加百分号
			}
			//居中显示
			//用最长成员的长度减去当前成员的长度
			//除以2再分别打印两次
			printf("%.*s", (INTERVAL - strlen(detail_log)) / 2, blank);
			printf("%s", detail_log);
			printf("%.*s|", ((INTERVAL - strlen(detail_log)) / 2) + ((1 == (strlen(detail_log) % 2)) ? 1 : 0), blank);     //若成员长度是偶数，则右边空格数比左边左边多一

																														   //计算true_num的累加值
			if (3 == j % 10 || 8 == j % 10)
			{
				true_num += (float)atof(detail_log);
			}

			//计算true_num的累加值
			if (4 == j % 10 || 9 == j % 10)
			{
				false_num += (float)atof(detail_log);
			}
		}
		putchar('\n');

	}

	//所有平均值存入数组
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
	printf("   |  平均数  |");
	for (i = 0; i < 4; i++)
	{
		sprintf(detail_log, "%.2f", average[i]);                   //转化为字符串才能居中显示
		if (1 == i)
		{
			strcat(detail_log, "%");                               //正确率后加百分号
		}
		printf("%.*s", (INTERVAL - strlen(detail_log)) / 2, blank);
		printf("%s", detail_log);
		printf("%.*s|", ((INTERVAL - strlen(detail_log)) / 2) + ((1 == (strlen(detail_log) % 2)) ? 1 : 0), blank);     //若成员长度是偶数，则右边空格数比左边左边多一
	}
	printf("\n   +----------+----------+----------+----------+----------+\n\n");
}

//判断是否为正整数，是返回true，否则返回false
bool estimate_positiveint(char str[])
{
	int i = 0;

	while ('\0' != str[i])
	{
		if (!isdigit(str[i++]))         //若字符串中有一个字符不是数字，则返回false
		{
			return false;
		}
	}
	return true;
}

//输入表达式求答案 
void solution()
{
	stack_i2p s_i2p;
	stack_cRP s_RP;
	char problem[ARRAYMAXSIZE];

	init_i2p(&s_i2p);
	init_cRP(&s_RP);
	printf("******************欢迎来到小学生算术作业系统*******************\n\n");
	printf("提示：(1) 如需退出，请输入Q!\n"
		"      (2)表达式中只能包含数字及以下符号：\n"
		"         '+'、'-'、'*'、'/'、'('、')'、'['、']'、'{'、'}'、'=''并且以' = '结束\n"
		"         例如：1+2-{[(3-4)*5]/6}+7=\n\n"
		"现在开始输入：\n\n");
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
			printf("输入错误，表达式中只能包含数字'+'、'-'、'*'、'/'、'('、')'、'['、']'、'{'、'}'、'=',并且以'='结束！\n请重新输入：\n");
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

//操作目录
int menu()
{
	int option;

	system("cls");
	printf("******************欢迎来到小学生算术作业系统*******************\n");
	printf("\t\t+----------------------------+\n"
		"\t\t|            目录            |\n"
		"\t\t+----------------------------+\n"
		"\t\t|        1.添加题库          |\n"
		"\t\t+----------------------------+\n"
		"\t\t|        2.练习              |\n"
		"\t\t+----------------------------+\n"
		"\t\t|        3.查看历史成绩      |\n"
		"\t\t+----------------------------+\n"
		"\t\t|        4.求答案            |\n"
		"\t\t+----------------------------+\n"
		"\t\t|        5.退出系统          |\n"
		"\t\t+----------------------------+\n\n"
		"请选择操作：");
	scanf("%d", &option);
	setbuf(stdin, NULL);
	while (1 != option && 2 != option && 3 != option && 4 != option && 5 != option)
	{
		printf("输入错误，请重新输入（正整数）：");
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
		fprintf(fproblems, "%d          \n", 0);                     //刚开始题库中的题目数为0
		fflush(fproblems);
		create_problems(fproblems);
		option = 0;
	}
	if (NULL == fscore && option != 5)
	{
		fscore = fopen("score.txt", "a");
		fclose(fscore);
		fscore = fopen("score.txt", "r+");
		fprintf(fscore, "%d          \n", 0);                     //刚开始有0条成绩记录
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

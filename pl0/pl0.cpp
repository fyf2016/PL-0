/*带详细注释的C语言版本PL/0编译程序
*编译和运行环境:
*1Visual C++6.0,VisualC++.NET and Visual C++.NET 2003
*WinNT, Win2000, WinXP and  Win2003
*2 gcc version 3.3.2  20031022(Red Hat Linux 3.3.2-1)
*Redhat Fedora core 1
*Intel 32 platform
*使用方法:
*运行后输入PL/0 源程序文件名
*回答是否输出虚拟机代码
*回答是否输出名字表
*fa.tmp 输出虚拟机代码
*fa1.tmp  输出源文件及其各行对应的首地址
*fa2.tmp  输出结果
*fas.tmp  输出名字表
*/
#include<stdio.h>
#include"pl0.h"
#include"string.h"
/*解释执行时使用的栈*/
#define stacksize 500
int main()
{
	bool nxtlev[symnum];
	printf("Input pl/0 file ?");
	scanf("%s",fname);                               /*输入文件名*/
	fin=fopen(fname,"r");
	if(fin)
	{
		printf("List object code ?(Y/N)");                /*是否输出虚拟机代码*/
		scanf("%s",fname);
		listswitch=(fname[0]=='y'||fname[0]=='Y');
		printf("List symbol table ? (Y/N)");              /*是否输出名字表*/
		scanf("%s",fname);
		tableswitch=(fname[0]=='y'||fname[0]=='Y');
		fa1=fopen("fa1.tmp","w");
		fprintf(fa1,"Iput pl/0 file ?");
		fprintf(fa1,"%s\n", fname);
		init();                                      /*初始化*/
		err=0;
		cc=cx=ll=0;
		ch=' ';
		if(-1!=getsym())/*成功读取第一个单词*/
		{
			fa=fopen("fa.tmp","w");
			fas=fopen("fas.tmp","w");
			addset(nxtlev,declbegsys,statbegsys,symnum);
			nxtlev[period]=true;
			if(-1==block(0,0,nxtlev))			/*调用编译程序*/
			{
				fclose(fa);
				fclose(fa1);
				fclose(fas);
				fclose(fin);/*编译过程未成功结束，关闭所有已打开的文件并返回*/
				printf("\n");
				return 0;
			}
			fclose(fa);
			fclose(fa1);
			fclose(fas);
			if(sym!=period)
			{
				error(9);
			}
			if(err==0)
			{
				fa2=fopen("fa2.tmp", "w");
				interpret();/*调用解释执行程序*/
				fclose(fa2);
			}
			else
			{
				printf("Errors in pl/0 program");
			}
		}
		fclose(fin);
	}
	else
	{
		printf("Can't open file! \n");
	}
	printf("\n");
	return 0;
}
/*
*初始化
*/
void init()
{
	int i;
	for(i=0;i<=255;i++)
	{
		ssym[i]=nul;
	}
	ssym['+']=plus;
	ssym['-']=minus;
	ssym['*']=times;
	ssym['/']=slash;
	ssym['(']=lparen;
	ssym[')']=rparen;
	ssym['=']=eql;
	ssym[',']=comma;
	ssym['.']=period;
	ssym['#']=neq;
	ssym[';']=semicolon;

	ssym['['] = lbrack;
	ssym[']'] = rbrack;
	ssym[':'] = colon;
	ssym['!'] = not;
	ssym['@'] = logic;
	ssym['%'] = mod;
	/*设置保留字名字,按照字母顺序,便于折半查找*/
	strcpy(&(word[0][0]),"begin");
	strcpy(&(word[1][0]),"call");
	strcpy(&(word[2][0]),"const");
	strcpy(&(word[3][0]),"do");
    strcpy(&(word[4][0]),"else");
	strcpy(&(word[5][0]),"end");
	strcpy(&(word[6][0]),"for");
	strcpy(&(word[7][0]),"if");
	strcpy(&(word[8][0]),"odd");
	strcpy(&(word[9][0]),"procedure");
	strcpy(&(word[10][0]),"read");
	strcpy(&(word[11][0]),"repeat");
	strcpy(&(word[12][0]),"then");
	strcpy(&(word[13][0]),"until");
	strcpy(&(word[14][0]),"var");
	strcpy(&(word[15][0]),"while");
	strcpy(&(word[16][0]),"write");
	/*设置保留字符号*/
	wsym[0]=beginsym;
	wsym[1]=callsym;
	wsym[2]=constsym;
	wsym[3]=dosym;
    wsym[4]=elsesym;
	wsym[5]=endsym;
	wsym[6]=forsym;
	wsym[7]=ifsym;
	wsym[8]=oddsym;
	wsym[9]=procsym;
	wsym[10]=readsym;
	wsym[11]=repeatsym;
	wsym[12]=thensym;
	wsym[13]=untilsym;
	wsym[14]=varsym;
	wsym[15]=whilesym;
	wsym[16]=writesym;
	/*设置指令名称*/
	strcpy(&(mnemonic[lit][0]),"lit");
	strcpy(&(mnemonic[opr][0]),"opr");
	strcpy(&(mnemonic[lod][0]),"lod");
	strcpy(&(mnemonic[sto][0]),"sto");
	strcpy(&(mnemonic[cal][0]),"cal");
	strcpy(&(mnemonic[inte][0]),"int");
	strcpy(&(mnemonic[jmp][0]),"jmp");
	strcpy(&(mnemonic[jpc][0]),"jpc");

	strcpy(&(mnemonic[lda][0]), "lda");
	strcpy(&(mnemonic[sta][0]), "sta");

	/*设置符号集*/
	for(i=0;i<symnum;i++)
	{
		declbegsys[i]=false;
		statbegsys[i]=false;
		facbegsys[i]=false;
	}
	/*设置声明开始符号集*/
	declbegsys[constsym]=true;
	declbegsys[varsym]=true;
	declbegsys[procsym]=true;
	/*设置语句开始符号集*/
    statbegsys[beginsym]=true;
	statbegsys[callsym]=true;
	statbegsys[forsym]=true;
	statbegsys[ifsym]=true;
	statbegsys[repeatsym]=true;
	statbegsys[whilesym]=true;
	/*设置因子开始符号集*/
	facbegsys[ident]=true;
	facbegsys[number]=true;
	facbegsys[lparen]=true;
	facbegsys[addadd]=true;     /*前置++*/
	facbegsys[subsub]=true;     /*前置--*/
	facbegsys[not]=true;        /*添加对!的识别*/
	facbegsys[logic]=true;		/*添加对@的识别*/
}
 /*
  *用数组实现集合的集合运算
  */
int inset(int e,bool* s)
{
    return s[e];/*返回e在数组s中的值*/
}
int addset(bool* sr,bool* s1,bool* s2,int n)
{
    int i;
    for(i=0;i<n;i++)
    {
       sr[i]=s1[i]||s2[i];/*逻辑或运算*/
    }
    return 0;
}
int subset(bool* sr,bool* s1,bool* s2,int n)
{
    int i;
    for(i=0;i<n;i++)
    {
       sr[i]=s1[i]&&(!s2[i]);
    }
    return 0;
}
int mulset(bool* sr,bool* s1,bool* s2,int n)
{
    int i;
    for(i=0;i<n;i++)
    {
       sr[i]=s1[i]&&s2[i];/*逻辑与运算*/
    }
    return 0;
}
/*
 *出错处理，打印出错位置和错误编码
 */
void error(int n)

{
	char space[81];
	memset(space,32,81); printf("-------%c\n",ch);
	space[cc-1]=0;/*出错时当前符号已经读完，所以cc-1*/
	printf("****%s!%d\n",space,n);
	switch(n)
	{
        case 1:
            printf("****常数说明中的“=”写成“：=”。\n");
            break;
        case 2:
            printf("****常数说明中的“=”后应是数字。\n");
            break;
        case 3:
            printf("****常数说明中的标识符后应是“=”。\n");
            break;
        case 4:
            printf("****const,var,procedure后应为标识符。\n");
            break;
        case 5:
            printf("****漏掉了“,”或“;“。\n");
            break;
        case 6:
            printf("****过程说明后的符号不正确(应是语句开始符,或过程定义符)\n");
            break;
        case 7:
            printf("****应是语句开始符。\n");
            break;
        case 8:
            printf("****程序体内语句部分的后跟符不正确。\n");
            break;
        case 9:
            printf("****程序结尾丢了句号“.”\n");
            break;
        case 10:
            printf("****语句之间漏了“;”。\n");
            break;
        case 11:
            printf("****标识符未说明。\n");
            break;
        case 12:
            printf("****赋值语句中，赋值号左部标识符属性应是变量。\n");
            break;
        case 13:
            printf("****赋值语句左部标识符后应是赋值号“:=”。\n");
            break;
        case 14:
            printf("****call后应为标识符。\n");
            break;
        case 15:
            printf("****call后标识符属性应为过程。\n");
            break;
        case 16:
            printf("****条件语句中丢了“then”。\n");
            break;
        case 17:
            printf("****丢了“end”或“;”。\n");
            break;
        case 18:
            printf("****while型循环语句中丢了“do”。\n");
            break;
        case 19:
            printf("****语句后的符号不正确。\n");
            break;
        case 20:
            printf("****应为关系运算符。\n");
            break;
        case 21:
            printf("****表达式内标识符属性不能是过程。\n");
            break;
        case 22:
            printf("****表达式中漏掉右括号“)”。\n");
            break;
        case 23:
            printf("****因子后的非法符号。\n");
            break;
        case 24:
            printf("****表达式的开始符不能是此符号。\n");
            break;
        case 30:
            printf("****常数越界。\n");
            break;
        case 31:
            printf("****表达式内常数越界。\n");
            break;
        case 32:
            printf("****嵌套深度超过允许值。\n");
            break;
        case 33:
            printf("****read或write或for语句中缺“）”。\n");
            break;
        case 34:
            printf("****read或write或for语句中缺“（”。\n");
            break;
        case 35:
            printf("****read语句括号中的标识符不是变量。\n");
            break;
	}
	err++;
}
/*
 *  漏掉空格，读取一个字符
 *
 *  每次读一行，存入line缓冲区，line被getsym取空后再读一行
 *
 *  被函数getsym调用
 */
int getch()
{
	if(cc==ll)  /*cc表示当前字符ch的位置，ll指向line缓冲区的行尾
            如果行缓冲区指针指向行缓冲区最后一个字符就从文件读一行到行缓冲区*/
    {
		if(feof(fin))   /*feof是检测流上的文件结束符
                        如果到达文件末尾*/
		{
			printf("program incomplete");   /*打印出错*/
			return -1;
		}
		ll=0;/*将指针调到初始位置*/
		cc=0;
		printf("%d ",cx );  /*虚拟机代码指针，取值范围[0,cxmax-1]*/
		fprintf(fa1,"%d ",cx);  /*fa1是一个文件，表示输出源文件及其各行对应的首地址，这里将cx指针位置写入fa1*/
		ch=' ';
		while(ch!=10)   /*10表示换行符，读到换行符，即为读出一整行*/
		{
            if(EOF==fscanf(fin,"%c",&ch))   /*EOF的值为-1*/
			{
				line[ll]=0; /*fscanf读取文件，遇到空置返回-1，表示文件结束*/
				break;
			}
			printf("%c",ch);
			fprintf(fa1,"%c",ch);   /*打印读出的字符，并写入文件*/
			line[ll]=ch;    /*将读到的字符写入缓冲区*/
			ll++;
		}
		printf("\n");
		fprintf(fa1,"\n");  /*打印和写入换行符*/
	}
	ch=line[cc];    /*读取一个缓冲区的字符到ch*/
	cc++;
	return 0;
}
 /*词法分析，获取一个符号
 */
int getsym()
{
	int i,j,k;
	while( ch==' '||ch==10||ch==9)  /*忽略空格、换行和TAB*/
	{
		getchdo;
	}
	if(ch>='a'&&ch<='z')    /*名字或保留字以a.z开头*/
	{
		k=0;
		do{
			if(k<al)
			{
				a[k]=ch;    /*a[k]临时符号，多出的一个字节用于存放0*/
				k++;
			}
			getchdo;
		}while(ch>='a'&&ch<='z'||ch>='0'&&ch<='9');
		a[k]=0;
		strcpy(id,a);   /*将a的值传递给id*/
		i=0;
		j=norw-1;   /*norw为关键字个数*/
		do{
			k=(i+j)/2;  /*折半查找的开始，确定当前符号是否为保留字*/
			if(strcmp(id,word[k])<=0)   /*如果当前的标识符小于k所指的保留字*/
			{
				j=k-1;
			}
			if(strcmp(id,word[k])>=0)
			{
				i=k+1;
			}

		}while(i<=j);
		if(i-1>j)
		{
			sym=wsym[k];    /*查找成功，是保留字*/
		}
		else
		{
			sym=ident;  /*查找失败，则是名字或数字*/
		}
	}
	else
	{
		if(ch>='0'&&ch<='9')    /*检测是否为数字：以0..9开头*/
		{
			k=0;    /*数字位数*/
			num=0;
			sym=number;
			do{
				num=10*num+ch-'0';  /*num * 10加上最近读出的字符的ASCII值减'0'的ASCII值得到相应的数值*/
				k++;
				getchdo;
			}while(ch>='0'&&ch<='9'); /*获取数字的值*/
			k--;
			if(k>nmax)  /*如果组成的数字位数大于最大允许的数字位数*/
			{
				error(30);
			}
		}
		else
		{	if(ch=='%') 
			{
				sym=mod;
			}
			if(ch=='!') 
			{
				sym=not;
			}
			if(ch=='@')
			{
				sym=logic;
			}
			if(ch==':')             /*检测赋值符号*/
			{
				getchdo;
				if(ch=='=')
				{
					sym=becomes;
					getchdo;
				}
				else
				{
					sym=colon;     /*不能识别的符号*/
					
				}
			}
			else
			{
				if(ch=='<')        /*检测小于或小于等于符号*/
				{
					getchdo;
					if(ch=='=')
					{
						sym=leq;
						getchdo;
					}
					else
					{
						sym=lss;
					}
				}
				else
				{
					if(ch=='>')        /*检测大于或大于等于符号*/
					{
						getchdo;
						if(ch=='=')
						{
							sym=geq;
							getchdo;
						}
						else
						{
						    sym=gtr;
						}
					}
					else
                    {
                        if(ch=='+')
						{
							getchdo;
							if(ch=='=')
							{
								sym=addequal;
								getchdo;
							}
							else if(ch=='+'){
								sym=addadd;
								getchdo;
							}
							else{
								sym=plus;
							}
						}
						else
						{
							if(ch=='-')
							{
								getchdo;
								if(ch=='=')
								{
									sym=subequal;
									getchdo;
								}
								else if(ch=='-'){
									sym=subsub;
									getchdo;
								}
								else{
									sym=minus;
								}
							}
                            else
                            {
                                if(ch=='*')
                                {
                                    getchdo;
                                    if(ch=='=')
                                    {
                                        sym=timeseql;
                                        getchdo;
                                    }
                                    else
                                    {
                                        sym=times;
                                    }
                                }
                                else
                                {
                                    if(ch=='/')
                                    {
                                        getchdo;
                                        if(ch=='=')
                                        {
                                            sym=slasheql;
                                            getchdo;
                                        }
                                        else if(ch=='*')/*添加注释*/
                                        {
                                            getchdo;
                                           // printf("注释/*%c",ch);
                                            while(1)
                                            {
                                                getchdo;
                                                //printf("%c",ch);
                                                if(ch=='*')
                                                {
                                                    getchdo;
                                                    if(ch=='/')
                                                    {
                                                       // printf("%c注释\n",ch);
                                                        break;
                                                    }
                                                    continue;
                                                }
                                            }
                                            getchdo;
                                        }
                                        else
                                        {
                                            sym=slash;
                                        }
                                    }
                                    else
                                    {
                                        sym=ssym[ch]; /*当符号不满足上述条件时，全部按照单字符号处理*/
                                        if(sym!=period)
                                        {
                                            getchdo;
                                        }
                                    }
                                }
                            }
						}
                    }
				}
			}
		}
	}
	return 0;
}
/*
*生成虚拟机代码
*
*x:instruction.f;
*y:instruction.l;
*z:instruction.a;
*/
int gen(enum fct x,int y,int z)
{
	if(cx>=cxmax)
	{
		printf("Program too long"); /*程序过长*/
		return -1;
	}
	code[cx].f=x;   /*把代码写入目标代码数组的当前cx所指位置*/
	code[cx].l=y;
	code[cx].a=z;
	cx++;
	return 0;
}
/*
*测试当前符号是否合法
*
*在某一部分（如一条语句，一个表达式）将要结束时时我们希望下一个符号属于某集合
*（该部分的后跟符号） test 负责这项检测，并且负责当检测不通过时的补救措施
*程序在需要检测时指定当前需要的符号集合和补救用的集合（如之前未完成部分的后跟
*符号），以及不通过时的错误号
*
*S1：我们需要的符号
*s2:如果不是我们需要的，则需要一个补救用的集合
*n:错误号
*/
int test(bool* s1,bool* s2,int n)
{
    if(! inset(sym,s1))
    {
		error(n);
		/*当检测不通过时，不停获取符号，直到它属于需要的集合或补救的集合*/
		while((! inset(sym,s1))&&(! inset(sym,s2)))
		{
			getsymdo;
		}
    }
    return 0;
}
/*
 *编译程序主体
 *
 *lev:当前分程序所在层
 *tx:名字表当前尾指针
 *fsys:当前模块后跟符号集合
 */
int block(int lev,int tx,bool* fsys)
{
    int i;
    int dx;                         /*名字分配到的相对地址*/
    int tx0;                        /*保留初始tx*/
    int cx0;                        /*保留初始cx*/
    bool nxtlev[symnum];            /*在下级函数的参数中，符号集合均为值参，但由于使用数组
                                      实现，传递进来的是指针，为防止下级函数改变上级函数的
                                      集合，开辟新的空间传递给下级函数*/
    dx=3;/*置初始值为3的原因是：每一层最开始的位置有三个空间，用于存放静态链SL、动态链DL和返回地址RA */
    tx0=tx;                         /*记录本层名字的初始位置*/
    table[tx].adr=cx;               /*符号表当前位置记下当前层代码的开始位置*/
    gendo(jmp,0,0);                 /*产生一行跳转指令，跳转位置暂时未知填0*/
    if(lev > levmax)
    {
		error(32);
    }
    do{
        if(sym==constsym)          /*收到常量声明符号，开始处理常量声明*/
        {
			getsymdo;
			do{
                constdeclarationdo(&tx,lev,&dx);    /*dx的值会被constdeclaration改变，使用指针*/
                while(sym==comma)
                {
                   getsymdo;
                   constdeclarationdo(&tx,lev,&dx);
                }
                if(sym==semicolon)
                {
                  getsymdo;
				}
				else
				{
					error(5);   /*漏掉了逗号或者分号*/
				}
			}while(sym==ident);
		}
		if(sym==varsym) /*收到变量声名符号，开始处理变量声名*/
		{
			getsymdo;
			do{
				vardeclarationdo(&tx,lev,&dx);
				while(sym==comma)
				{
					getsymdo;
					vardeclarationdo(&tx,lev,&dx);
				}
				if(sym==semicolon)
				{
					getsymdo;
				}
				else
				{
					error(5);
				}
			}while(sym==ident);
		}
		while(sym==procsym) /*收到过程声名符号，开始处理过程声名*/
		{
			getsymdo;
			if(sym==ident)
			{
				enter(procedur,&tx,lev,&dx);    /*记录过程名字*/
				getsymdo;
			}
			else
			{
				error(4);   /*procedure后应为标识符*/
			}
			if(sym==semicolon)
			{
				getsymdo;
			}
			else
			{
				error(5);   /*漏掉了分号*/
			}
			memcpy(nxtlev,fsys,sizeof(bool)*symnum);
			nxtlev[semicolon]=true;
			if(-1==block(lev+1,tx,nxtlev))
			{
				return -1;  /*递归调用*/
			}
            if(sym==semicolon)
            {
                getsymdo;
                memcpy(nxtlev,statbegsys,sizeof(bool)*symnum);
                nxtlev[ident]=true;
                nxtlev[procsym]=true;
                testdo(nxtlev,fsys,6);  /*检查当前符号是否合法，不合法则用fsys恢复语法分析同时抛6号错*/
             }
             else
             {
                 error(5);                       /*漏掉了分号*/
             }
        }
        memcpy(nxtlev,statbegsys,sizeof(bool)*symnum);
        nxtlev[ident]=true;
        nxtlev[period]=true;
        testdo(nxtlev,declbegsys,7);    /*检查当前状态是否合法，不合法则用声明开始符号作出错恢复、抛7号错*/
    }while(inset(sym,declbegsys));                /*直到没有声明符号*/
    code[table[tx0].adr].a=cx;     /*把前面生成的跳转语句的跳转位置改成当前位置，开始生成当前过程代码*/
    table[tx0].adr=cx;                          /*当前过程代码地址*/
    table[tx0].size=dx;                         /*声明部分中每增加一条声明都会给dx增加1,声明部分已经结束,dx就是当前过程数据的size*/
    cx0=cx;
    gendo(inte,0,dx);                          /*生成分配内存代码，分配dx个空间*/
    if(tableswitch)                            /*输出名字表*/
    {
        printf("TABLE:\n");
        if(tx0+1>tx)
        {
			printf("NULL\n");
        }
        for(i=tx0+1;i<=tx;i++)
        {
            switch(table[i].kind)
            {
                case constant:
					printf("%d const %s",i,table[i].name);
					printf("val=%d\n",table[i].val);
					fprintf(fas,"%d const %s",i,table[i].name);
					fprintf(fas,"val=%d\n",table[i].val);
                    break;
                case variable:
                    printf("%d var %s",i,table[i].name);
                    printf(" lev=%d addr=%d\n",table[i].level,table[i].adr);
                    fprintf(fas,"%d var %s",i,table[i].name);
                    fprintf(fas,"lev=%d addr=%d\n",table[i].level,table[i].adr);
                    break;
                case procedur:
                    printf("%d proc%s",i,table[i].name);
                    printf("lev=%d addr=%d size=%d\n",table[i].level,table[i].adr,table[i].size);
					fprintf(fas,"%d proc%s",i,table[i].name);
					fprintf(fas,"lev=%d adr=%d size=%d \n",table[i].level,table[i].adr,table[i].size);
					break;
				case arrays:
					printf("%d array %s ", i, table[i].name);
					printf("lev=%d addr=%d size=%d\n", table[i].level, table[i].adr, table[i].size);
					fprintf(fas, "%d array %s ", i, table[i].name);
					fprintf(fas, "lev=%d addr=%d size=%d\n", table[i].level, table[i].adr, table[i].size);
			}
		}
		printf("\n");
	}
	/*语句后跟符号为分号或end*/
	memcpy(nxtlev,fsys,sizeof(bool)*symnum);    /*每个后跟符号集和都包含上层后跟符号集和，以便补救*/
	nxtlev[semicolon]=true;
	nxtlev[endsym]=true;
	statementdo(nxtlev,&tx,lev);
	gendo(opr,0,0);     /*每个过程出口都要使用的释放数据段命令*/
	memset(nxtlev,0,sizeof(bool)*symnum);   /*分程序没有补救集合*/
	test(fsys,nxtlev,8);                  /*检测后跟符号正确性*/
	listcode(cx0);                          /*输出代码*/
	return 0;
}
/*
*在名字表中加入一项
*
*k:名字种类const,var or procedure
*ptx:名字表尾指针的指针，为了可以改变名字表尾指针的数值
*lev:名字所在的层次，以后所有的lev都是这样
*pdx:为当前应分配的变量的相对地址，分配后要增加1
*/
void enter (enum object k,int *ptx,int lev, int *pdx)
{
	(*ptx)++;
	strcpy(table[(*ptx)].name,id);       /*全局变量id中已存有当前名字的名字*/
	table[(*ptx)].kind=k;
	switch(k)
	{
		case constant:               /*常量名字*/
			if (num>amax)
			{
				error(31);          /*数越界*/
				num=0;
			}
			table[(*ptx)].val=num;  /*如是合法的数值，就登陆到符号表*/
			break;
		case variable:                /*变量名字*/
			table[(*ptx)].level=lev;    /*记下它所属的层次号*/
			table[(*ptx)].adr=(*pdx);   /*记下它在当前层中的偏移量*/
			(*pdx)++;
			break;                  /*过程名字*/
		case procedur:
			table[(*ptx)].level=lev;    /*记录下这个过程所在层次*/
			break;
		case arrays:     /* 数组名,进行记录下界等 */
			table[(*ptx)].level = lev;
			table[(*ptx)].adr = (*pdx);
			table[(*ptx)].data = g_arrBase;
			table[(*ptx)].size = g_arrSize;
			*pdx = (*pdx)+g_arrSize;
			break;
	}

}
/*
 *查找名字的位置
 *找到则返回在名字表中的位置，否则返回0
 *
 *idt: 要查找的名字
 *tx:：当前名字表尾指针
 */
int position(char *  idt,int  tx)
{
	int i;
	strcpy(table[0].name,idt);  /*先把id放入名字表0号位置*/
	i=tx;
	while(strcmp(table[i].name,idt)!=0)
	{
		i--;
	}
	return i;
}
/*
 *常量声明处理
 */
int constdeclaration(int *  ptx,int lev,int *  pdx)
{
	if(sym==ident)
	{
		getsymdo;
		if(sym==eql ||sym==becomes)
		{
			if(sym==becomes)
			{
				error(1);                  /*把=写出成了：=*/
			}
			getsymdo;
			if(sym==number)
			{
				enter(constant,ptx,lev,pdx);    /*把这个常量登陆到名字表*/
				getsymdo;
			}
			else
			{
				error(2);                  /*常量说明=后应是数字*/
			}
		}
		else
		{
			error(3);                      /*常量说明标识后应是=*/
		}
	}
	else
	{
		error(4);                         /*const后应是标识*/
	}
	return 0;
}
/*
 *
 */
//加入数组声明
int vardeclaration(int * ptx, int lev, int * pdx)
{
	int arrayRet=-1;
    if (sym==ident)
    {
        arrayRet=arraydeclaration(ptx,lev,pdx); /* 先判断数组 */
        switch(arrayRet)
        {
        case 1:
            enter(arrays,ptx,lev,pdx); // 填写数组名
			getsymdo;
            break;
        case 0:
            enter(variable,ptx,lev,pdx); // 填写名字表
            //getsymdo;
            break;
        default:
            return -1;  /* 数组定义解析出错 */
        }
    }
    else
    {
        error(4);   /* var后应是标识 */
    }
    return 0;
}
 /*
  *输出目标代码清单
  */
void listcode(int cx0)
{
	int i;
   	if (listswitch)
   	{
   		for(i=cx0;i<cx;i++)
   	 	{
   	 		printf("%d %s %d %d\n",i,mnemonic[code[i].f],code[i].l,code[i].a);
   	 		fprintf(fa,"%d %s %d %d\n",i,mnemonic[code[i].f],code[i].l,code[i].a);
   	    }
   	 }
}
/*
*语句处理
*/
int statement(bool* fsys,int * ptx,int lev)
{
	int i,cx1,cx2,cx3,cx4,cx5;;
   	bool nxtlev[symnum];
   	if(sym==ident)              /*准备按照赋值语句处理*/
   	{
		i=position(id,*ptx);    /*查找名字所对应的table位置 */
   	 	if(i==0)
   	 	{
   	 		error(11);          /*变量未找到 */
   	 	}
   	 	else
   	 	{
   	 		if((table[i].kind != variable)&&(table[i].kind != arrays))
   	 		{
   	 		 	error(12);          /*赋值语句格式错误 */
   	 		 	i=0;               /*i置0作为错误标志*/
   	 		}
   	 		else
   	 		{
   	 			
				enum fct fct1 = sto;
				switch(table[i].kind)
				{
				case arrays:
					arraycoefdo(fsys, ptx, lev);
					fct1 = sta;  /* 数组保存,要多读一个栈 */
							/* go through */
				case variable:
				{	getsymdo;
					
   	 				if(sym==becomes)
   	 				{
   	 					getsymdo;
						memcpy(nxtlev,fsys,sizeof(bool)* symnum);
						expressiondo(nxtlev,ptx,lev);   /*处理赋值符号右侧表达式*/
						if(i!=0)
						{                                   /*expression将执行一系列指令，但最终结果
															将会保存在栈顶，执行sto命令完成赋值*/
							gendo(fct1,lev-table[i].level,table[i].adr);
						}
   	 				}
					else if(sym==mod)       /*检测到%符号*/
					{
						getsymdo;
						//例如a%b = a - (a/b)*b 
						//将a的值入栈
						gendo(lod,lev-table[i].level,table[i].adr);  /*找到变量地址并将其值入栈*/
					
						if(sym==semicolon)
						{
							getsymdo;
						}
						memcpy(nxtlev,fsys,sizeof(bool)* symnum);
						expressiondo(nxtlev,ptx,lev);
						//将a和b的值相除
						gendo(opr,0,5);
						//再取b的值到栈顶
						gendo(lod,lev-table[i+1].level,table[i+1].adr);
						gendo(opr,0,4);
						gendo(lod,lev-table[i].level,table[i].adr);
						gendo(opr,0,3);
						gendo(opr,0,1);
		
						if(i!=0)
						{
							gendo(fct1,lev-table[i].level,table[i].adr);
						}
					}
					else if(sym==addequal)       /*检测到+=符号*/
					{
						getsymdo;
						gendo(lod,lev-table[i].level,table[i].adr);  /*找到变量地址并将其值入栈*/
						if(sym==semicolon)
						{
							getsymdo;
							printf("+=后面直接跟了分号");
						}
						memcpy(nxtlev,fsys,sizeof(bool)* symnum);
						expressiondo(nxtlev,ptx,lev);
						gendo(opr,0,2);
						if(i!=0)
						{
							gendo(fct1,lev-table[i].level,table[i].adr);
						}
					}
					else if(sym==subequal)  /*检测到-=符号*/
					{
							getsymdo;
							gendo(lod,lev-table[i].level,table[i].adr);    /*找到变量地址并将其值入栈*/
							if(sym==semicolon)
							{
								getsymdo;
							}
							memcpy(nxtlev,fsys,sizeof(bool)* symnum);
							expressiondo(nxtlev,ptx,lev);
							gendo(opr,0,3);
							if(i!=0)
							{
								gendo(fct1,lev-table[i].level,table[i].adr);
							}
					}
					else if(sym==timeseql)   /*检测到*=符号*/
					{
						getsymdo;
						gendo(lod,lev-table[i].level,table[i].adr);    /*找到变量地址并将其值入栈*/
						if(sym==semicolon)
						{
							getsymdo;
						}
						memcpy(nxtlev,fsys,sizeof(bool)* symnum);
						expressiondo(nxtlev,ptx,lev);
						gendo(opr,0,4);
						if(i!=0)
						{
							gendo(fct1,lev-table[i].level,table[i].adr);
						}
					}
					else if(sym==slasheql)  /*检测到/=符号*/
					{
						getsymdo;
						gendo(lod,lev-table[i].level,table[i].adr);    /*找到变量地址并将其值入栈*/
						if(sym==semicolon)
						{
							getsymdo;
						}
						memcpy(nxtlev,fsys,sizeof(bool)* symnum);
						expressiondo(nxtlev,ptx,lev);
						gendo(opr,0,5);
						if(i!=0)
						{
							gendo(fct1,lev-table[i].level,table[i].adr);
						}
					}
					else if(sym==addadd)    /*检测到后置++符号*/
					{
						getsymdo;
						if(i!=0)
						{
							gendo(lod,lev-table[i].level,table[i].adr);
							gendo(lit,0,1);
							gendo(opr,0,2);
							gendo(fct1,lev-table[i].level,table[i].adr);
						}
					}
					else if(sym==subsub)  /*检测到后置--符号*/
					{
						getsymdo;
						if(i!=0)
						{
							gendo(lod,lev-table[i].level,table[i].adr);
							gendo(lit,0,1);
							gendo(opr,0,3);
							gendo(fct1,lev-table[i].level,table[i].adr);
						}
					}
					else
   	 				{
   	 			 		error(13);      /*没有检测到赋值符号*/
   	 				}
				}
					break;
				default:
					error(12);
					i=0;
					break;
				}
   	 		}
        }
    }
    else
    {

		if(sym==logic) /*检测逻辑取反符号@*/
		{
			getsymdo;
			if(sym==ident)
			{
				 i=position(id,*ptx);
                 if(i==0)
                 {
                    error(11);
                 }
				 else
				 {
					 if(table[i].kind!=variable)
                    {  
                        error(12);
                        i=0;
                    }
                    else
                    { 
						getsymdo;
						gendo(lod,lev-table[i].level,table[i].adr);
						gendo(lit,0,0);
						gendo(opr,0,8);
						gendo(sto,lev-table[i].level,table[i].adr);      
                           
                    }
					
				 }
			}
		
		}

		if(sym==not) 
		{
			getsymdo;
			if(sym==ident)
			{
				 i=position(id,*ptx);
                 if(i==0)
                 {
                    error(11);
                 }
				 else
				 {
					 if(table[i].kind!=variable)
                    {  
                        error(12);
                        i=0;
                    }
                    else
                    { 
						getsymdo;
						gendo(lod,lev-table[i].level,table[i].adr);
						gendo(opr,0,1);
						gendo(sto,lev-table[i].level,table[i].adr);      
                           
                    }
					
				 }
			}
		
		}
       if(sym==addadd) /*检测到前置++符号*/
        {
            getsymdo;
            if(sym==ident)  /*后面跟的是变量*/
            {
                 i=position(id,*ptx);
                 if(i==0)
                 {
                    error(11);
                 }
                 else
                 {
                    if(table[i].kind!=variable)
                    {   /*++后没跟变量，出错*/
                        error(12);
                        i=0;
                    }
                    else
                    { /*++后跟变量，处理生成中间代码*/
                            getsymdo;
                            gendo(lod,lev-table[i].level,table[i].adr);/*先取值到栈顶*/
                            gendo(lit,0,1);              /*将值入栈*/
                            gendo(opr,0,2);     /*加法，即+1，栈顶加次栈顶*/
                            gendo(sto,lev-table[i].level,table[i].adr);/*出栈取值到内存*/
                    }
                }
            }
        }
        else if(sym==subsub)    /*检测到前置--符号*/
         {
            getsymdo;
            if(sym==ident)    /*后面跟的是变量*/
            {
                i=position(id,*ptx);
                if(i==0)
                {
                    error(11);
                }
                else
                {
                    if(table[i].kind!=variable)   /*--后没跟变量，出错*/
                    {
                        error(12);
                        i=0;
                    }
                    else        /*--后跟变量，处理生成中间代码*/
                    {
                        if(table[i].kind==variable) /*后跟变量*/
                        {
                            getsymdo;
                            gendo(lod,lev-table[i].level,table[i].adr);/*先取值到栈顶*/
                            gendo(lit,0,1);       /*将值为入栈*/
                            gendo(opr,0,3);      /*减法，即-1，栈顶减次栈顶*/
                            gendo(sto,lev-table[i].level,table[i].adr);/*出栈取值到内存*/
                        }
                    }
                }
            }
         }
       
         if(sym == forsym)
		{
			getsymdo;
			if(sym != lparen)  error(34);//没有左括号出错
			else 
			{
				getsymdo;
				statementdo(nxtlev, ptx, lev);  //S1代码
				if(sym != semicolon)  error(10); //语句缺少分号出错
				else
				{
					cx1=cx;
					getsymdo;
					conditiondo(nxtlev, ptx, lev);   //E代码
					if(sym != semicolon)  error(10);  //语句缺少分号出错
					else 
					{	cx2=cx;
						gendo(jpc,0,0);
						cx3=cx;
						gendo(jmp,0,0);
						getsymdo;
						cx4=cx;
						statementdo(nxtlev, ptx, lev);	//S2代码
						if(sym != rparen)  error(22);  //缺少右括号出错
						else 
						{
							gendo(jmp,0,cx1);
							getsymdo;
							cx5=cx;
							statementdo(nxtlev, ptx, lev);  //S3代码
							code[cx3].a=cx5;
							gendo(jmp,0,cx4);
							code[cx2].a=cx;
						}
					}
				}
			}
        }
        else if(sym==repeatsym)
        {
            cx1=cx;	 /*保存当前指令地址*/
            getsymdo;
            memcpy(nxtlev,fsys,sizeof(bool)*symnum);
            nxtlev[untilsym]=true;
            statementdo(fsys,ptx,lev);
            if(sym==semicolon)
            {
                getsymdo;
                if(sym==untilsym)
                {
                    getsymdo;
                    conditiondo(fsys,ptx,lev);
                    gendo(jpc,0,cx1);/*经condition处理后，cx1为repeat后循环语句的位置，条件为假时一直循环*/
                }
            }
            else
            {
                error(5);
            }
        }
        else
        {
            if(sym==readsym)        /*准备按照read语句处理*/
            {
                getsymdo;
                if(sym!=lparen)
                {
                   error(34);       /*格式错误，应是左括号*/
                }
                else
                {
                    do{
                        getsymdo;
                        if(sym==ident)
                        {
                            i=position(id, *ptx);       /*查找要读的变量*/
                        }
                        else
                        {
                            i=0;
                        }
                        if(i==0)
                        {
                            error(35);              /*read()中应是声明过的变量名*/
                        }
                        else
                        {
                            gendo(opr,0,16);        /*生成输入指令，读取值到栈顶*/
                            gendo(sto,lev-table[i].level,table[i].adr);	/* 储存到变量*/
                        }
                        getsymdo;
                    }while (sym==comma);	/*一条read语句可读多个变量 */
                }
                if(sym!=rparen)
                {
                    error(33);			/* 格式错误，应是右括号*/
                    while(!inset(sym,fsys)) /* 出错补救，直到收到上层函数的后跟符号*/
                    {
                        getsymdo;
                    }
                }
                else
                {
                    getsymdo;
                }
            }
            else
            {
                if(sym==writesym)			/* 准备按照write语句处理，与read类似*/
                {
                    getsymdo;
                    if(sym==lparen)
                    {
                        do{
                            getsymdo;
                            memcpy(nxtlev,fsys,sizeof(bool)*symnum);
                            nxtlev[rparen]=true;
                            nxtlev[comma]=true;		/* write的后跟符号为）or，*/
                            expressiondo(nxtlev,ptx,lev);   /* 调用表达式处理，此处与read不同，read为给变量赋值*/
                            gendo(opr,0,14);    /* 生成输出指令，输出栈顶的值*/
                        }while(sym==comma);
                        if(sym!=rparen)
                        {
                            error(33);  /* write()中应为完整表达式*/
                        }
                        else
                        {
                            getsymdo;
                        }
                    }
                    gendo(opr,0,15);		/* 输出换行*/
                }
                else
                {
                    if(sym==callsym)		/* 准备按照call语句处理*/
                    {
                        getsymdo;
                        if(sym!=ident)
                        {
                            error(14);    /*call后应为标识符*/
                        }
                        else
                        {
                            i=position(id,*ptx);
                            if(i==0)
                            {
                                error(11); /*过程未找到*/
                            }
                            else
                            {
                                if(table[i].kind==procedur)     /*如果这个标识符为一个过程名*/
                                {
                                    gendo(cal,lev-table[i].level,table[i].adr);  /*生成call指令*/
                                }
                                else
                                {
                                    error(15);  /*call后标识符应为过程*/
                                }
                            }
                            getsymdo;
                        }
                    }
                    else
                    {
                        if(sym==ifsym)         /*准备按照if语句处理*/
                        {
                            getsymdo;
                            memcpy(nxtlev,fsys,sizeof(bool)*symnum);
                            nxtlev[thensym]=true;
                            nxtlev[dosym]=true;    /*后跟符号为then或do*/
                            conditiondo(nxtlev,ptx,lev);   /*调用条件处理（逻辑运算）函数*/
                            if(sym==thensym)
                            {
                                getsymdo;
                            }
                            else
                            {
                                error(16);             /*缺少then*/
                            }
                            cx1=cx;                  /*保存当前指令地址*/
                            gendo(jpc,0,0);            /*生成条件跳转指令，跳转地址暂写0*/
                            statementdo(fsys,ptx,lev);   /*处理then后的语句*/
                            if(sym==semicolon)
                            {
                                getsymdo;
                                if(sym==elsesym)/*then语句后出现else*/
                                {
                                    getsymdo;
                                    cx2=cx;
                                    code[cx1].a=cx+1;   /*cx为当前的指令地址，
                                                      cx+1即为then语句执行后的else语句的位置，回填地址*/
                                    gendo(jmp,0,0);
                                    statementdo(fsys,ptx,lev);
                                    code[cx2].a=cx;     /*经statement处理后，cx为else后语句执行
                                                        完的位置，它正是前面未定的跳转地址，回填地址*/
                                }
                                else
                                {
								
                                    code[cx1].a=cx;         /*经statement处理后，cx为then后语句执行
                                                        完的位置，它正是前面未定的跳转地址*/
                                }
                            }
                            else
                            {
                                error(5);
                            }
                        }
                        else
                        {
                            if(sym==beginsym)   /*准备按照复合语句处理*/
                            {
                                getsymdo;
                                memcpy(nxtlev,fsys,sizeof(bool)*symnum);
                                nxtlev[semicolon]=true;
                                nxtlev[endsym]=true;/*后跟符号为分号或end*/
                                /*循环调用语句处理函数，直到下一个符号不是语句开始符号或收到end*/
                                statementdo(nxtlev,ptx,lev);
                                while(inset(sym,statbegsys)||sym==semicolon)
                                {
                                    if(sym==semicolon)
                                    {
                                        getsymdo;
                                    }
                                    else
                                    {
                                        error(10);/*缺少分号*/
                                    }
                                    statementdo(nxtlev,ptx,lev);
                                }
                                if(sym==endsym)
                                {
                                    getsymdo;
                                }
                                else
                                {
                                    error(17); /*缺少end或分号*/
                                }
                            }
                            else
                            {
                                if(sym==whilesym)/*准备按照while语句处理*/
                                {
                                    cx1=cx;        /*保存判断条件超作的位置*/
                                    getsymdo;
                                    memcpy(nxtlev,fsys,sizeof(bool)*symnum);
                                    nxtlev[dosym]=true; /*后跟符号为do*/
                                    conditiondo(nxtlev,ptx,lev);  /*调用条件处理*/
                                    cx2=cx;       /*保存循环体的结束的下一个位置*/
                                    gendo(jpc,0,0); /*生成条件跳转，但跳出循环的地址未知*/
                                    if(sym==dosym)
                                    {
                                        getsymdo;
                                    }
                                    else
                                    {
                                        error(18);      /*缺少do*/
                                    }
                                    statementdo(fsys,ptx,lev); /*循环体*/
                                    gendo(jmp,0,cx1);   /*回头重新判断条件*/
                                    code[cx2].a=cx;   /*反填跳出循环的地址，与if类似*/
                                }
                                else
                                {
                                    memset(nxtlev,0,sizeof(bool)*symnum);/*语句结束无补救集合*/
                                    testdo(fsys,nxtlev,19);/*检测语句结束的正确性*/
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}
/*
*表达式处理
*/
int expression(bool*fsys,int*ptx,int lev)
{
	enum symbol addop;              /*用于保存正负号*/
	bool nxtlev[symnum];
	if(sym==plus||sym==minus)        /*开头的正负号，此时当前表达式被看作一个正的或负的项*/
	{
		addop=sym;                 /*保存开头的正负号*/
		getsymdo;
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);    /*从fsys拷贝n个字节到nxtlev*/
		nxtlev[plus]=true;
		nxtlev[minus]=true;
		termdo(nxtlev,ptx,lev);         /*处理项*/
		if(addop==minus)
		{
			gendo(opr,0,1);           /*如果开头为负号生成取负指令*/
		}
	}
	else                             /*此时表达式被看作项的加减*/
	{
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[plus]=true;
		nxtlev[minus]=true;
		//nxtlev[mod]=true;
		termdo(nxtlev,ptx,lev);          /*处理项*/
	}
	while(sym==plus||sym==minus)
	{
		addop=sym;
		getsymdo;
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[plus]=true;
		nxtlev[minus]=true;
		termdo(nxtlev,ptx,lev);              /*处理项*/
		if(addop==plus)
		{
			gendo(opr,0,2);                /*生成加法指令*/
		}
		else
		{
			 gendo(opr,0,3);               /*生成减法指令*/
		}
	}
	return 0;
}
/*
*项处理
*/
int term(bool*fsys,int *ptx,int lev)
{
    enum symbol mulop;               /*用于保存乘除法符号*/
    bool nxtlev[symnum];
    memcpy(nxtlev,fsys,sizeof(bool)*symnum) ;
    nxtlev[times]=true;
    nxtlev[slash]=true;
//	nxtlev[mod]=true;
    factordo(nxtlev,ptx,lev);            /*处理因子*/
    while(sym==times||sym==slash)
    {
		mulop=sym;
        getsymdo;
        factordo(nxtlev,ptx,lev);
        if(mulop==times)
        {
            gendo(opr,0,4);          /*生成乘法指令*/
        }
        else
        {
            gendo(opr,0,5);           /*生成除法指令*/
        }
    }
     return 0;
}
/*
*因子处理
*/
int factor(bool*fsys,int *ptx,int lev)
 {
    int i;
    bool nxtlev[symnum];
    testdo(facbegsys,fsys,24);           /*检测因子的开始符好号*/
    while(inset(sym,facbegsys))         /*循环直到不是因子开始符号*/
    {
        if(sym==ident)               /*因子为常量或者变量*/
        {
            i=position(id,*ptx);        /*查找名字*/
            if(i==0)
            {
                error(11);            /*标识符未声明*/
            }
            else
            {
				switch(table[i].kind)
				{
					case constant:                               /*名字为常量*/
						gendo(lit,0,table[i].val);                   /*直接把常量的值入栈*/
						break;
					case variable:                               /*名字为变量*/
						gendo(lod,lev-table[i].level,table[i].adr);      /*生成lod指令，
                            把位于距离当前层level的层的偏移地址为adr的变量的值放到栈顶*/
						break;
					case procedur:                               /*名字为过程*/
						error(21);                               /*不能为过程*/
						break;
					case arrays:     /* 名字为数组名 */
						arraycoefdo(fsys,ptx,lev);
						gendo(lda,lev-table[i].level,table[i].adr);   /* 找到变量地址并将其值入栈 */
						break;
				}
			}
			getsymdo;
			if(sym==addadd)     /*因子出现b:=a++类型*/
            {
                gendo(lit,lev-table[i].level,1);            /*将值入栈*/
                gendo(opr,lev-table[i].level,2); /*加法，即+1，栈顶加次栈顶*/
                gendo(sto,lev-table[i].level,table[i].adr); /*出栈取值到内存*/
                gendo(lod,lev-table[i].level,table[i].adr); /*取值到栈顶*/
                gendo(lit,0,1);
                gendo(opr,0,3);                             /*栈顶值减*/
                getsymdo;
            }
            else if(sym==subsub)  /*因子出现b:=a--类型*/
            {
                gendo(lit,lev-table[i].level,1);            /*将值入栈*/
                gendo(opr,lev-table[i].level,3);            /*减法，即-1，栈顶减次栈顶*/
                gendo(sto,lev-table[i].level,table[i].adr); /*出栈取值到内存*/
                gendo(lod,lev-table[i].level,table[i].adr);
                gendo(lit,0,1);
                gendo(opr,0,2);                             /*栈顶值加*/
                getsymdo;
            }
		}
	
		else if(sym==logic) 
		{
	
			getsymdo;
            if(sym==ident)
            {
				//getsymdo;
                i=position(id,*ptx);
                if(i==0)
                {
                    error(11);
                }
				else 
				{
					
                    gendo(lod,lev-table[i].level,table[i].adr);/*先取值到栈顶*/
					gendo(lit,0,0);
                    gendo(opr,0,8);/*栈顶和次栈顶相比较*/
                    gendo(sto,lev-table[i].level,table[i].adr);/*出栈取值到内存*/
                 
				}
			}
			
		}
		else if(sym==not) 
		{
			getsymdo;
            if(sym==ident)
            {
				//getsymdo;
                i=position(id,*ptx);
                if(i==0)
                {
                    error(11);
                }
				else 
				{
                    gendo(lod,lev-table[i].level,table[i].adr);/*先取值到栈顶*/
                    gendo(opr,0,1);/*取反*/
                    gendo(sto,lev-table[i].level,table[i].adr);/*出栈取值到内存*/
                 
				}
			}
			
		}
		else if(sym==addadd)    /*因子出现b:=++a类型*/
        {
            getsymdo;
            if(sym==ident)
            {
                getsymdo;
                i=position(id,*ptx);
                if(i==0)
                {
                    error(11);
                }
                else
                {
                    if(table[i].kind==variable)  /*变量*/
                    {                     /*先加后再用a*/
                        gendo(lod,lev-table[i].level,table[i].adr);/*先取值到栈顶*/
                        gendo(lit,0,1);/*将值入栈*/
                        gendo(opr,0,2);/*加法，即+1，栈顶加次栈顶*/
                        gendo(sto,lev-table[i].level,table[i].adr);/*出栈取值到内存*/
                        gendo(lod,lev-table[i].level,table[i].adr);/*取值到栈顶*/
                    }
                }
            }
        }
        else if(sym==subsub)    /*因子出现b:=--a类型*/
        {
            getsymdo;
            if(sym==ident)
            {
                getsymdo;
                i=position(id,*ptx);
                if(i==0)
                {
                    error(11);
                }
                else
                {
                    if(table[i].kind==variable)  /*变量*/
                    {              /*先减后再用a*/
                        gendo(lod,lev-table[i].level,table[i].adr);/*先取值到栈顶*/
                        gendo(lit,0,1);                /*将值入栈*/
                        gendo(opr,0,3);                /*减法，即-1，栈顶减次栈顶*/
                        gendo(sto,lev-table[i].level,table[i].adr);/*出栈取值到内存*/
                        gendo(lod,lev-table[i].level,table[i].adr); /*取值到栈顶*/
                    }
                }
            }
        }
        else
        {
            if(sym==number)              /*因子为数*/
            {
                if(num>amax)
                {
                    error(31);
                    num=0;
                }
                gendo(lit,0,num);
                getsymdo;
            }
            else
            {
                if(sym==lparen)            /*因子为表达式*/
                {
                    getsymdo;
                    memcpy(nxtlev,fsys,sizeof(bool)*symnum);
                    nxtlev[rparen]=true;
                    expressiondo(nxtlev,ptx,lev);
                    if(sym==rparen)
                    {
                        getsymdo;
                    }
                    else
                    {
                        error(22);         /*缺少右括号*/
                    }
                }
                testdo(fsys,facbegsys,23);    /*因子后有非法符号*/
            }
}
    }
	return 0;
}
/*
 条件处理*/
int condition(bool* fsys,int* ptx,int lev)
{
    enum symbol relop;
    bool nxtlev[symnum];
    if(sym==oddsym)                        /*准备按照odd运算处理*/
   	{
		getsymdo;
		expressiondo(fsys,ptx,lev);
		gendo(opr,0,6);                     /*生成odd指令*/
    }
    else
    {
		memcpy(nxtlev,fsys,sizeof(bool)*symnum);
		nxtlev[eql]=true;  /*=*/
		nxtlev[neq]=true;  /*#*/
		nxtlev[lss]=true;  /*<*/
		nxtlev[leq]=true;  /*<=*/
		nxtlev[gtr]=true;  /*>*/
		nxtlev[geq]=true;  /*>=*/
		expressiondo(nxtlev,ptx,lev);
		if(sym!=eql&&sym!=neq&&sym!=lss&&sym!=leq&&sym!=gtr&&sym!=geq)
		{
			error(20);
		}
		else
		{
			relop=sym;
			getsymdo;
			expressiondo(fsys,ptx,lev);
			switch(relop)
			{
				case eql:
					gendo(opr,0,8); /*等号：产生8号判等指令*/
					break;
				case neq:
					gendo(opr,0,9); /*不等号：产生9号判不等指令*/
					break;
				case lss:
					gendo(opr,0,10); /*小于号：产生10号判小于指令*/
					break;
				case geq:
					gendo(opr,0,11); /*大于等于号：产生11号判不小于指令*/
					break;
				case gtr:
					gendo(opr,0,12); /*大于号：产生12号判大于指令*/
					break;
				case leq:
					gendo(opr,0,13); /*小于等于号：产生13号判不大于指令*/
					break;
			}

		}
    }
    return 0;
}
/*解释程序*/
void interpret()
{
	int p,b,t;                /*指令指针，指令基址，栈顶指针*/
	struct instruction i;       /*存放当前指令*/
	int s[stacksize];          /*栈*/
	printf("start pl0\n");
	t=0;
	b=0;
	p=0;
	s[0]=s[1]=s[2]=0;
	do{
		i=code[p];         /*读当前指令*/
		p++;
		switch(i.f)
		{
			case lit:        /*将a的值取到栈顶*/
				s[t]=i.a;
				t++;
				break;
			case opr:       /*数字、逻辑运算*/
				switch(i.a)
				{
					case 0:         /*释放内存*/
						t=b;
						p=s[t+2];
						b=s[t+1];
						break;
					case 1:
						s[t-1]=-s[t-1]; /*取负*/
						break;
					case 2:
						t--;
						s[t-1]=s[t-1]+s[t];/*加法*/
						break;
					case 3:
						t--;
						s[t-1]=s[t-1]-s[t];/*减法*/
						break;
					case 4:
						t--;
						s[t-1]=s[t-1]*s[t];/*乘法*/
						break;
					case 5:
						t--;
						s[t-1]=s[t-1]/s[t];/*除法*/
             			break;
					case 6:
						s[t-1]=s[t-1]%2;  /*奇偶判断，奇数为真，偶数为假*/
						break;
					case 8:
						t--;
						s[t-1]=(s[t-1]==s[t]);/*判断是否相等*/
 						break;
					case 9:
						t--;
						s[t-1]=(s[t-1]!=s[t]);/*判断是否不等*/
 						break;
					case 10:
						t--;
						s[t-1]=(s[t-1]<s[t]);/*判断是否小于*/
 						break;
					case 11:
						t--;
						s[t-1]=(s[t-1]>=s[t]);/*判断是否大于等于*/
 						break;
					case 12:
						t--;
						s[t-1]=(s[t-1]>s[t]);/*判断是否大于*/
 						break;
					case 13:
						t--;
						s[t-1]=(s[t-1]<=s[t]);/*判断是否小于等于*/
 						break;
					case 14:
						printf("%d",s[t-1]);/*次栈顶值输出到屏幕*/
						fprintf(fa2,"%d",s[t-1]);
						t--;
						break;
					case 15:
						printf("\n");       /*输出换行符到屏幕*/
						fprintf(fa2,"\n");
						break;
					case 16:
						printf("?");        /*从命令行读入一个输入至栈顶*/
						fprintf(fa2,"?");
						scanf("%d",&(s[t]));
						fprintf(fa2,"%d\n",s[t]);
						t++;
						break;
				}
				break;
			case lod:       /*取相对当前过程的数据基地址为ａ的内存的值到栈顶*/
				s[t]=s[base(i.l,s,b)+i.a];
				t++;
				break;
			case sto:       /*栈顶的值存到相对当前过程的数据基地址为ａ的内存*/
				t--;
	            s[base(i.l,s,b)+i.a]=s[t];
				break;
			case cal:                /*调用子程序*/
				s[t]=base(i.l,s,b); /*将父过程基地址入栈*/
				s[t+1]=b;           /*将本过程基地址入栈，此两项用于base函数*/
				s[t+2]=p;           /*将当前指令指针入栈*/
				b=t;               /*改变基地址指针值为新过程的基地址*/
				p=i.a;              /*跳转*/
				break;
			case inte:               /*分配内存*/
				t+=i.a;
				break;
			case jmp:              /*直接跳转*/
				p=i.a;
				break;
			case jpc:              /*条件跳转*/
				t--;
				if(s[t]==0)
				{
					p=i.a;
				}
	    		break;
			case lda:   /* 数组元素访问,当前栈顶为元素索引,执行后,栈顶变成元素的值 */
				s[t-1] = s[base(i.l,s,b) + i.a + s[t-1]];
				break;
			case sta:   /* 栈顶的值存到数组中,索引为次栈顶 */
				t-=2;
				s[base(i.l,s,b) + i.a + s[t]] = s[t+1];
				break;
		}
	}while (p!=0);
}
/*通过过程基址求上1层过程的基址*/
int base(int l,int * s,int b)
{
	int b1;
	b1=b;
	 while(l>0)
	 {
		 b1=s[b1];/*用当前层数据区基址中的内容（上一层的基址）作为新的当前层，即向上找了一层*/
     	 l--;
	 }
	 return b1;
}


/*
* 数组声明处理, 下界和上界允许已经定义过的常量标识符
*/
int arraydeclaration(int* ptx, int lev, int* pdx)
{
    char arrId[al];             /* 暂存数组标识名,避免被覆盖 */
    int cstId;                  /* 常量标识符的位置 */
    int arrBase=-1, arrTop=-1;  /* 数组下界、上界的数值 */
	getsymdo;
    if(sym==lbrack)  /* 标识符之后是'[',则识别为数组 */
    {
        strcpy(arrId, id);
        //getchdo;

        /* 检查下界 */
        getsymdo;
        if(sym==ident)
        {
            if((cstId=position(id,(*ptx)))!=0)
                arrBase=(constant==table[cstId].kind)?table[cstId].val:-1;
        }
        else
        {
            arrBase=(sym==number)?num:-1;
        }
        if(-1==arrBase)
		{
			error(50);
			return -1;
		}
        
        /* 检查冒号 */
		getsymdo;
		if(sym!=colon)
        {
            error(50);
			return -1;
        }
        //getchdo;
        
        /* 检查上界 */
        getsymdo;
        if(sym==ident)
        {
            if((cstId=position(id,(*ptx)))!=0)
				arrTop=(constant==table[cstId].kind)?table[cstId].val:-1;
        }
        else
        {
			arrTop=(number==sym)?num:-1;
        }
        if(arrTop==-1)
		{
			error(50);	//随意指定,因为原程序对错误号的规划极差!
			return -1;
		}

        /* 检查']' */
        getsymdo;
        if(sym!=rbrack)
        {
            error(50);
			return -1;
        }

        /* 上下界是否符合条件检查 */
        g_arrSize=arrTop-arrBase+1;
        g_arrBase=arrBase;        
        if(g_arrSize<=0)
		{
			error(50);
			return -1;
		}
        
        /* 恢复数组的标识符 */
        strcpy(id, arrId);
        return 1;
    }
    return 0;
}


/*
* 数组元素索引计算与“虚拟机”生成
*/
int arraycoef(bool *fsys,int *ptx,int lev)
{
    bool nxtlev[symnum];
    int i = position(id,*ptx);
    getsymdo;
    if (sym==lbrack) /* 索引是括号内的表达式 */
    {
        getsymdo;
        memcpy(nxtlev,fsys,sizeof(bool)*symnum);
        nxtlev[rbrack]=true;
        expressiondo(nxtlev,ptx,lev);
        if (sym==rbrack)
        {
            gendo(lit,0,table[i].data);
            gendo(opr,0,3);   /* 系数修正,减去下界的值 */
            return 0;
        }
        else
        {
            error(22);  /* 缺少右括号 */
        }
    }
    else
    {
        error(51);  /* 数组访问错误 */
    }
    return -1;
}
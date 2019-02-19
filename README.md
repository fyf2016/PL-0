# PL/0
[![HitCount](http://hits.dwyl.io/fyf2016/PL-0.svg)](http://hits.dwyl.io/fyf2016/PL-0) [![GitHub license](https://img.shields.io/github/license/fyf2016/PL-0.svg)](https://github.com/fyf2016/PL-0/blob/master/LICENSE) [![Download](https://img.shields.io/badge/downloads-master-orange.svg)](https://codeload.github.com/PL-0/zip/master) 
[![Gitter](https://img.shields.io/gitter/room/fyf2016/PL-0.svg)](https://gitter.im/PL-0/community?utm_source=share-link&utm_medium=link&utm_campaign=share-link)

[项目开发流程和演示 ( 点击即可跳转 )](https://fyf2016.github.io/2018/08/22/%E7%BC%96%E8%AF%91%E5%8E%9F%E7%90%86%E8%AF%BE%E8%AE%BE/)

>PL/0 程序功能的扩充( 编译原理课设 )

## 项目简介
    本次编译原理课程设计的题目是扩展 pl/0 编译程序的功能, 通过对原始pl/0
    编译程序的解读, 了解C语言版本的Pl/0编译程序的运作原理以及各部分函数
    的功能及其相互调用关系, 并在此基础上扩充了包括一位数组的处理, for循环语句
    和repeat-until循环语句的处理...等功能, 并对这些功能进行了测试。
    
 >注: 本项目中包含了所有测试文件(".txt为后缀的文件") , 逻辑取反和取余运算的测试文件依次为logic.txt和mod.txt
 
## 扩充的主要的功能

1. 增加语句for(<语句>；<条件>；<语句>)；	
2. 扩充语句if <条件> then <语句> else <语句>；	
3. 增加语句repeat <语句> until <条件>；	
4. 增加自增自减运算	
5. 增加+=，-=，*=，/=运算；	
6. 增加一维数组的处理 ( 例如:  a[1:2] )	
7. 增加取反功能( 算术取反和逻辑取反 )	
8. 增加错误提示	
9. 增加注释处理功能	
10. 增加取余( % )运算	

## 各部分功能测试
1. 测试for循环	
(1) 测试文件
```pascal
var a,i;
begin
     a:=0;
     for(i:=1;i<=3;i:=i+2)
     begin
   	 a:=a+i;
     end;
     write(a);
end. 
```
    第一次循环,i=1, 1<3, a=0+1 = 1
    第二次循环,i=3, 3=3, a=1+3 = 4
    第三次,i=5 >3 跳出循环
    故a的正确值应为 4

(2) 运行结果

<img src="https://fyf2016.github.io/images/project/compiling/rs1.png" width="230" style="margin-left: 25px;">

2. 测试if-then-else循环	
(1) 测试文件
```pascal
var a,b;
begin
      a:=0;
      read(b);
      if b<=3
      then
          write(b);
      else
          write(a);
end.
```
当输入的b的值小于等于3时,输出b的值,
否则输出a的值,即0

(2) 运行结果
>当输入的值小于等于3时

<img src="https://fyf2016.github.io/images/project/compiling/rs2.png" width="230" style="margin-left: 25px;">

>当输入的值大于3时

<img src="https://fyf2016.github.io/images/project/compiling/rs3.png" width="230" style="margin-left: 25px;">

3. 测试repeat-until循环	
(1) 测试文件
```pascal
var x;
begin
       x:=1;
       repeat
             x:=x+1;
       until x>5;
       write(x);
end.
```
 当x的值大于5时才输出, 应输出6

(2) 运行结果

<img src="https://fyf2016.github.io/images/project/compiling/rs4.png" width="230" style="margin-left: 25px;">

4. 测试++，--功能	
(1) 测试文件
```pascal
var i,a;
begin
   i:=1;
   i++;
   write(i); 
   ++i;
   write(i);
   i--;
   write(i);
   --i;
   write(i);
   a:=i++;
   write(a);
   a:=++i;
   write(a);
   a:=i--;
   write(a);
   a:=--i;
   write(a);
end
```

	i初始值为1, 执行i++后, i的值变为2
	执行++i后, i的值又变为3
	执行i - - 后, i的值变为 2
	执行 - - i后, i的值变为1
	执行a:=i++ 后,a的值变为1, i的值变为2
	执行a:=++i 后,a的值变为3, i的值变为3
	执行a:=i-- 后,a的值变为3, i的值变为2
	执行a:= --i后,a的值变为1, i的值变为1

(2) 运行结果

<img src="https://fyf2016.github.io/images/project/compiling/rs5.png" width="230" style="margin-left: 25px;display: block;">

5. 测试+=、-=、*=、/=功能	
(1) 测试文件
```pascal
var a,b,c;
begin
       a:=1;
       b:=2;
       c:=3;
       b+=a;
       c-=b;
       write(b);
       write(c);
       a+=1;
       b*=a;
       write(b);
       c+=b;
     c/=a;
     write(c);
end.
```

		初始值a为1, b为2, c为3
		执行b+=a后, b的值变为3
		执行c - = b后, c的值变为0
		执行a+=1后, a的值变为2
		执行b*= a后, b的值变为6
		执行c+=b后, c的值变为6
		执行c/=a后, c的值变为3

(2) 运行结果

<img src="https://fyf2016.github.io/images/project/compiling/rs6.png" width="230" style="margin-left: 25px;display: block;">

6. 测试一维数组处理功能
(1) 测试文件
```pascal
var a[1:2];
begin
      a[1]:=1;
      a[2]:=2;
      a[1]:=a[1]*3;
      write(a[1]);
      write(a[2]);
end.
```
 	初始化数组a[1:2], a[1]初始值为1, a[2]初始值为2,
 	执行a[1]:=a[1]*3 后a[1]的值变为3

 (2) 运行结果

<img src="https://fyf2016.github.io/images/project/compiling/rs7.png" width="230" style="margin-left: 25px;display: block;">

7. 测试取反功能(算术取反和逻辑取反)	
> 算术取反:

(1) 测试文件
```pascal
var a,b,c;
begin
   a:= -5;
   c:=3;
   b:=!a;
   !c;
   write(b);
   write(c);
end.
```
	 a的初始值为 -5, c的初始值为3, 执行b=!a 后,
	 b的值变为5, 执行 !c 后, c的值变为 - 3

(2) 运行结果

<img src="https://fyf2016.github.io/images/project/compiling/rs8.png" width="230" style="margin-left: 25px;display: block;">

>逻辑取反:

(1) 测试文件
```pascal
var a,b,c;
begin
       a:=0;
       b:= -3;
       c:=@b;
       @a;
       @b;
       write(a);
       write(b);
       write(c);
end.
```
	  a的初始值为0 , b的初始值为-3, 执行c:=@b 后,
	  b的值变为0 , c的值也变为0 , 执行@a 后, a的值
	  变为1, 执行@b 后, b的值变为1, 故最后应输出 1, 1 , 0
(2) 运行结果

<img src="https://fyf2016.github.io/images/project/compiling/rs9.png" width="230" style="margin-left: 25px;display: block;">

8. 测试错误提示功能	
(1) 测试文件
```pascal
var a,b
begin
   a=-5;
   b:=3;
   write(a);
   write(b);
end.
```
第一句定义变量的语句缺少分号,
给变量a赋值的语句 := 写成了= 

(2) 运行结果
<img src="https://fyf2016.github.io/images/project/compiling/rs10.png" width="230" style="margin-left: 25px;display: block;">

9. 测试注释处理功能	
(1) 测试文件
```pascal
var x;
begin
       x:=1;
       repeat
          x:=x+1;
       until x>5;   /* 这是注释 */
       write(x); 
end.
```
(2) 运行结果

<img src="https://fyf2016.github.io/images/project/compiling/rs11.png" width="230" style="margin-left: 25px;display: block;">

10. 测试取余(%)运算	
(1) 测试文件
```pascal
var a,b,c,;
begin
     a:=16;
     b:=3;
     c:= -2;
     a%b;
     c%b;
     write(a);
     write(c);
end.
```
		a的初始值为16 , b的初始值为3, c的初始值为 -2 , 执行
		a%b后, a的值变为1,执行c%b后, c的值为- 2,所以应该输
		出1, -2

(2) 运行结果

<img src="https://fyf2016.github.io/images/project/compiling/rs12.png" width="230" style="margin-left: 25px;display: block;">

## 各功能详细解读
> 前往以下网址即可获取
[功能解读 ( 点击即可跳转 )](https://fyf2016.github.io/2018/08/22/%E7%BC%96%E8%AF%91%E5%8E%9F%E7%90%86%E8%AF%BE%E8%AE%BE/)


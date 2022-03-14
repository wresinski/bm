## KMP算法 ##
#### 1. KMP算法的核心在于构造出一个next数组，当发生失配时，不需要回退主串指针，同时也不需要归零模式串指针，而是依据next数组进行回退

代码实现如下：
```cpp
int indexKMP(char *s, int sLength, char *t, int tLength, int pos. int *next) {
    int i = pos;
    int j = 0;
    while (i < sLength && j < tLength) {
		if (j == -1 || s[i] == t[j]) { // 当j=-1或者主串和模式串对应位字符相等时，i和j都右移一位
			i++;
			j++;
		} else // 否则，主串i不变，模式串j依据next数组回退
			j = next[j];
    }
    if (j >= tLength)
	return i - tLength;
    else
	return -1;
}
```
#### 2. next数组的构造

当第j个字符匹配失败，由前0~j-1个字符组成的串记为S，则：next[j] = S的最长相等前后缀长度
$$ 
next[j]=\left\{
\begin{aligned}
& -1\quad当j=1时 \\
& Max\{k|1<k<j且'{p_0\dots p_{k-1}}'='{p_{j-k}\dots p_{j-1}}'\}\quad当此集合不为空时\\
& 0\quad其他情况（前后缀不匹配）
\end{aligned}
\right.
$$
构建next数组的步骤（递推法）：  
假设当前`next[j]=k`，则表明当前一定有以下关系：$'{p_0\dots p_{k-1}}'='{p_{j-k}\dots p_{j-1}}'$，其中$k$为满足$1<k<j$能取到的最大$k$值；  
此时存在两种可能：  
(1) 若$p_k=p_j$，则表明$'{p_0\dots p_k}'='{p_{j-k＋1}\dots p_j}'$，即`next[j+1]=k+1`  
(2) 若$p_k\neq\;p_j$，则表明$'{p_0\dots p_k}'\neq\;'{p_{j-k＋1}\dots p_j}'$，此时相当于$'{p_0\dots p_k}'$与$'{p_{j-k＋1}\dots p_j}'$在$p_k$和$p_j$上发生失配，即需要求$'{p_0\dots p_{k-1}}'$的最长相等前后缀长度，aka`next[k]`！  
步骤如下：
1. 如果$p_k\neq\;p_j$，则令`k'=next[k]`
2. 如果$p_{k'}=p_j$，`next[j+1]=k'+1`即`next[j+1]=next[k]+1`；否则令`k=k'`，返回第1步如此循环，直至$p_j$和模式中某个字符匹配成功或者不存在任何$k'$满足条件，则`next[j+1]=1`

代码实现如下：
```cpp
void getNext(char *t, int tLength, int *next) {
    next[0] = -1;
    next[1] = 0;
    int i = 1;
    int j = next[i];
    i++;
    while (i < tLength) {
		if (t[i - 1] == t[j]) {
			next[i] = ++j;
			i++;
		} else if (j == 0) {
			next[i] = j;
			i++;
		} else
			j = next[j];
    }
}
```
#### 3. nextval数组的构造

当子串和模式串不匹配时`j=nextval[j];`

代码实现如下：
```cpp
void getNextVal(char *t, int tLength, int *next) {
    next[0] = -1;
    next[1] = 0;
    int i = 1;
    int j = next[j];
    i++;
    while (i < tLength) {
		if (t[i - 1] == t[j]) {
			/*******优化判断*******/
			if (t[i] != t[++j])
				next[i] = j;
			else
				next[i] = next[j];
			/********************/
			i++;
		} else if (j == 0) {
			next[i] = j;
			i++;
		} else
			j = next[j];
    }
}
```
#### 4.复杂度分析
* 时间复杂度：$O(n)$，其中$n$是字符串s的长度
* 空间复杂度：$O(n)$

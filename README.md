# Community Structure in Networks

This project take a graph as an input and and divide it to **clusters** according to the edges density. 

project assume:
- the input is an indirect graph
- the adjacency matrix of the graph is a ***sparse matrix***, that's mean the edges number are O(n). If this assumption does not exist, the running time will be significantly **slower for large n** .
- Input file as describe below.
- two start arguments- "input file path" "output file path"
- a number is positive if it is bigger than 0.00001

The division base on [Newman, M. E. J. (2006). Modularity and community structure in networks](https://www.pnas.org/content/103/23/8577/tab-article-info) article. 

## Files Format
The input and output files are both binary files consisting only of integers (each vertex has a 'name' represented with indices, for n vertexes the names are 0,1,...,n-1).

### Input file
The **first value** represents the number of nodes in the network.
The **second value** represents the number of edges of the first node, i.e., k1. It is followed by the k1 indices of its neighbors, in increasing order.
The **next value** is k2, followed by the k2 indices of the neighbors of the second node, then k3 and its k3 neighbors, and so on until node n-1.

### Output file
The **first value** represents the number of groups in the division.
The **second value** represents the number of nodes in the first group, followed by the indices of the nodes in the group, in increasing order.
The **next value** is the number of nodes in the second group, followed by the indices of the nodes in the group, in increasing order, and so on until the last group.

# Parameters
**"The Algorithms"** section below describe the algorithms and their pseudo-code that were used. It will use the following parameter:

**A** = the adjacency matrix.
**k** = a vector of degrees, for example, k[0] equals the number of neighbors of vertex 0.
**n** = number of vertexes.
**M** = $\sum_{i=0}^n{k[i]}$
**g** = a group of vertexes, for example {0,5,9}.
**B** = a matrix, $B_{ij}={A_{ij}-\frac{k_ik_j}{M}}$
**s** = $\pm{1}$ vector that represent the division into two.
**Q** = modularity value = $\frac{1}{2}\sum_{i,j}{(B_{ij})}s_is_j$ = $\frac{1}{2}s^TBs$

>$\delta{_{ij}}$ equal 1 if i==j , else 0.

**B[g]**=a matrix, $B[g]_{ij\in{g}}={B_{ij\in{g}}}-\delta{_{ij}}\sum_{d\in{g}}B_{id}$

>$f_i^g=\sum_{j\in{g}}B[g]_{ij}$

**$\hat{B[g]}$** = a matrix, $B[g]_{ij}-\delta_{ij}f_i^g$

**$\Delta{Q}$** =  $\frac{1}{2}s^T\hat{B[g]}s$

for the mathematic explanation for this formulas, you may read the article.

# The Algorithms

## Power Iteration
Power iteration is an eigenvalue algorithm: given a symmetric matrix, **the algorithm produces the eigenvector corresponding to its greatest absolute eigenvalue**.

The power iteration algorithm starts with a random vector $v_0$, and in each iteration uses the current vector to produce a new vector, used in the next iteration. When done, the vector produced in the final iteration is the desired eigenvector.

In the power iteration algorithm, in each iteration we obtain the next normalized vector according to the following equation: $v_{k+1} =\frac {Dv_k}{||Dv_k||}$
(D is our input symmetric matrix and $v_k$ is the current vector). 

We are done iterating when the change in the new vector is small enough. When all of the differences (between each corresponding pair of values in the previous vector and the new one) are smaller than epsilon, in this project, epsilon=0.00001.

## Matrix Shifting

Let $C$ be a matrix with the eigenvalues $\{\lambda_1,...,\lambda_n\}$, and let the 1-norm of C be the sum of its largest column: $||C||_1=max_j({\sum_i|C_{ij}|})$
Let $C'=||C||_1\cdot{I}+C$, then $C'$ has the same eigenvectors as $C$ and the eigenvalues of $C'$ are $\{\lambda_1+||C||_1,...,\lambda_n+||C||_1\}$. Thus, the **dominant eigenpair** of $C'$ is $(\lambda_1+||C||_1, v_1)$, and the **leading eigenpair** of C is $(\lambda_1, v_1)$.

## Divide to two
***pseudo-code*** *Algorithm 1 Divide a network into two modules*
1. Compute leading eigenpair u1 and b1 of the modularity matrix B
2. if $(b1 \le 0)$:
2.1. The network is indivisible
3. Compute $s=\{s_1,...,s_n\}$ where $s_i\in\{+1,-1\}$ , according to u1 sign.
4. if $(s^TBs \le 0)$:
4.1. The network is indivisible
5. return a division into two groups according to s

***explanation***
1. Compute the eigenpair with *power iteration* and *matrix shifting*. Power iteration approximates the **dominant eigenpair**; however, in the division algorithm we search for the **leading eigenpair**. That is, we search for the eigenpair with the largest eigenvalue, while power iteration provides us with the eigenpair with the largest absolute eigenvalue .
In order to correct this, we will use matrix shifting to ensure the eigenpair we find is the leading eigenpair.
2. that's mean the best division is no division - to **compute the eigenvalue** we use $\lambda=v_k\cdot{Dv_k}$ formula.
>note, $v_k$ is normalize due to the power iteration
3. s represent the division, each vertex is +1 or -1, represent the two groups
4. if the modularity is not positive, the previous division (no division) is better. 
5. return the division according to s vector. 

## Modularity Maximization
*This is an optimize algorithm used in order to get a better division into two groups.*

The algorithm find a vertex v that, when moved the other group, will give the **biggest increase in modularity** of the complete network, or the smallest decrease if no increase is possible, and **move it to the other group**. This process repeats with the constraint that each vertex may only be moved once, until all vertices have been moved. Once done, from all the states of division into two groups during the operation, **find the state that has the maximal modularity**, and start again from this state. We repeat the entire process iteratively until no further improvement is found, i.e., until the maximal state is the current one.

***pseudo-code*** *Algorithm 4 improve a 2-division of the network*
**Input:** s- a $\pm$ 1 vector representing the initial 2-division of a network with $n_g$ vertices
1. **repeat**
2. Unmoved $\leftarrow\{0,1,...,n_g-1\}$
3. $x\leftarrow{B[g]s}$
3.1. **for** $i=0$ to $n_g-1$ **do**
3.1.1. $score[i] \leftarrow{-2(s_ix_i+\frac{k_i^2}{M})}$ 
3.2. **end for**
4. $j'\leftarrow{argmax\{score[j]:j\in{Unmoved}\}}$
4.1. $s_{j'}=-s_{j'}$
4.2. $improve[i]\leftarrow{score[j']}$
4.3. $Unmoved\leftarrow{Unmoved/\{j'\}}$
5. **for** $i=1$ to $n_g-1$ **do**
5.1. **for all** $k\in{Unmoved}$ **do**
5.1.1. $score[i]\leftarrow{score[i]-4s_is_{j'}B[g]_{ij'}}$
5.2. **end for**
.
5.2. $j'\leftarrow{argmax\{score[j]:j\in{Unmoved}\}}$
5.3 $s_{j'}=-s_{j'}$
5.4 $improve[i]\leftarrow{improve[i-1]+score[j']}$
5.5. $Unmoved\leftarrow{Unmoved/\{j'\}}$
6. **end for**
 .
7. $i'\leftarrow{argmax\{improve[i]:i\}}$
8. **for** $i=n_g-1$ down to $i'+1$ **do**
8.1. $j\leftarrow{indices[i]}$
8.2. $s_j=-s_j$
9. **end for**
10. **if** $i'=n_g-1$ **then**
10.1. $\Delta{Q}=0$
11. **else**
11.1. $\Delta{Q}\leftarrow{improve[i']}$
12. **until** $\Delta{Q}$ is non positive 

## Divide a group into two

In the previous section we described a method that divides a network into two groups. Many networks, however, contain more than two communities, thus we would like to find good divisions of networks into a larger number of groups.
The standard approach to this problem, which we will adopt, is **repeated division into two**. We divide the network into two parts, then divide each of those parts, and so forth. There for we take $\hat{B[g]}$ for the calculation. **g** is a group with the vertexes of the group we want to divide.
the algorithm is algorithm 1 with this small change (we preform the algorithm only on part of the vertexes). 
>NOTE: This algorithm gives **a** division with good modularity, **not necessary the best modularity**. The pseudo-code below already contain the modularity maximization algorithm in order to make a further maximization to the division. 
>
***pseudo-code*** *Algorithm 2 Divide a group into two modules*
1. Compute leading eigenpair u1 and b1 of the modularity matrix $\hat{B[g]}$
2. if $(b1 \le 0)$:
2.1. The network is indivisible
3. Compute $s=\{s_1,...,s_n\}$ where $s_i\in\{+1,-1\}$ , according to u1 sign.
4. Algorithm 4 (modularity maximization)
5. if $(s^T\hat{B[g]}s \le 0)$:
4.1. The network is indivisible
6. return a division into two groups according to s


## Divide a network to clusters
This algorithm use Algorithm 2 repeatedly in order to divide the network to groups

***pseudo-code*** *Algorithm 3 Divide a network into modularity groups*
1. Start with a trivial division into one group: $P\leftarrow\{\{0,...,n-1\}\}$
2. Start with an empty output set of groups: O$\leftarrow\{\}$
3. **Repeat** until P is empty:
3.1. Remove a group g from P
3.2. Divide g into g1, g2 with Algorithm 2
3.3. **if** either g1 or g2 is of size 0:
3.3.1. Add g to O
3.4. **else**:
3.4.1. Add to O: any group (g1 and/or g2) of size 1
3.4.2. Add to P: any group (g1 and/or g2) larger than 1
4. Output the division given by O

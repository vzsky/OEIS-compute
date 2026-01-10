---
created: 09 Jan 2026
---

Erdos 421 asks if there is an increasing sequence with density 1 such that all consecutive products are unique. We consider the greedy sequence starting from 2. 

# Naive computation
The sequence is defined with $a(n)$ being the smallest integer larger than $a(n-1)$ such that all consecutive products are unique. Say we have a sequence $a(1), \ldots, a(n)$, we then check if we can add $a(n)+1$, if not, check if we can add $a(n) + 2$ and so on. When checking if we can add $k$ to a sequence $a(1) \ldots, a(n)$, we only need to check all the *end products* against all the products in $a$. That is we check if there is $x, y, z$ such that $$\prod_{i=x}^y a(i) = k \cdot \prod_{i=z}^{n} a(i)$$
If there is, then we cannot add $k$, otherwise $a(n+1) := k$. We then repeat this process indefinitely.
# Optimization 
Now, we will try to optimize the computation of *"can we add $k$ to $a(1), \ldots, a(n)$"*.
### No compute for primes
If $k$ is prime, then we can always add $k$ to the sequence as the product containing prime will be unique.
### Stopping at primes
Instead of checking all pairs, we only need to checks up to the largest prime under $k$. Say $p := a(r)$ is prime for some $r \le n$, then we only need to check 
$$\prod_{i=x}^y a(i) = k \cdot \prod_{i=z}^{n} a(i)$$
for all $z > r$. 

To show that, let $(x, y, z)$ be indices satisfying the equation where $z$ is maximized. If $z \le r$ then $p$ divides the right hand side. This means $p$ must divide the left hand side. So $x \le r \le y$, so $z \le y$, which means there must be another set of answer $(x, y-1, z+1)$ contradicting the maximality of $z$.
## Looking at the target
Now, let $$T := k \cdot \prod_{i=z}^n a(i)$$ be the target and focusing on finding $(x, y)$ satisfying $\prod_{i=x}^y a(i) = T$. 

Let's also factor $T$ as $$T = p_1^{m_1} \cdots p_s^{m_s}$$
Write $\pi(x, y) := \prod_{i=x}^y a(i)$ and write $a^{-1}(k)$ as the first index $i$ such that $a(i) \ge k$.
### Optimization 1
Assume $p_i > m_i$.
For each $p_i$, since $p_i^{m_i} \vert T$, the solution $(x, y)$ we want it must satisfies $p_i^{m_i} \vert \pi(x, y)$, the smallest such product will need to be at least at high as $\pi(x,y)$ where $a(x) = p_i$ and $a(y) = m_i \cdot p_i$

Therefore, if for any $i$, the product $\pi(a^{-1}(p_j), a^{-1}(m_j \cdot p_j))$ exceed $T$, we can stop the search as it will be impossible to find any solution.
> it might make sense to check divisibility rather than size here

### Optimization 2
Assume $s > 1$. We know that $p_s \vert T$ so the solution must satisfies $p_s \vert \pi(x, y)$. However for $i \ne s$, as $p_i \vert T$ we must also have $p_i \vert \pi(x, y)$. 

Let $p_s \equiv a \pmod{p_i}$ where $0 < a < p_i$ we know that $p_s - a$ and $p_s - a + p_i$ are divisible by $p_i$. They are also the closest (in each direction) number from $p_s$ that is divisible by $p_i$.

So let $f$ be the minimum $p_s - a$ across all $i$ and $g$ be the maximum $p_s - a + p_i$, then $$\pi(x, y) > \max(\pi(a^{-1}(f), a^{-1}(p_s)), \pi(a^{-1}(p_s), a^{-1}(g)))$$ so calculating the right hand side, if that exceed $T$ we can skip.

### Optimization 3
If $T$ is a product of $t$ terms ($t = n - z + 1$), then we know that the solution $(x,y)$ must satisfies $y - x + 1 > t$ since $a(y) < k$. We also know that $p_s \vert T$ so the answer is at least $\pi(a^{-1}(p_s) - t - 1, a^{-1}(p_s))$ 

This should just be merged with Optimization 2

# Further ideas: 
- the bottleneck seems to be memory now going pass 10^7
    - hold skipped instead of sequence should help a lot


# MandleBrot Set
https://en.wikipedia.org/wiki/Mandelbrot_set

# wikipedia definition

**Formal definition**

The Mandelbrot set is the uncountable set of values of $c$ in the complex plane 
for which the orbit of the critical point $z = 0$ under iteration of the quadratic map

$$z \mapsto z^2 + c$$

remains bounded. Thus, a complex number $c$ is a member of the Mandelbrot set if, 
when starting with $z_0 = 0$ and applying the iteration repeatedly, the absolute 
value of $z_n$ remains bounded for all $n \in \mathbb{Z}^+$.

For example, for $c = 1$, the sequence is 0, 1, 2, 5, 26, ..., (sequence A003095 
in the OEIS) which tends to infinity, so 1 is not an element of the Mandelbrot set. 
On the other hand, for $c = -1$, the sequence is 0, -1, 0, -1, 0, ..., which is bounded, 
so -1 does belong to the set.

The Mandelbrot set can also be defined as the connectedness locus of the family 
of quadratic polynomials $f(z) = z^2 + c$, the subset of the space of parameters 
$c$ for which the Julia set of the corresponding polynomial forms a connected set. 
In the same way, the boundary of the Mandelbrot set can be defined as the bifurcation 
locus of this quadratic family, the subset of parameters near which the dynamic 
behavior of the polynomial (when it is iterated repeatedly) changes drastically.

## running simulation
a pre-built binary is provided, use either

'''
./mandelbrot_set
'''

or

'''
make run
'''

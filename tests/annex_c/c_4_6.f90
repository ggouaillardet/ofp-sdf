integer :: j, k, jj(5)
real :: x(10), xx(10,10), yy(10,10)

! C.4.6    Examples of FORALL statements
! Example 1:

     FORALL (J=1:M, K=1:N) xx(K, J) = yy(J, K)
     FORALL (K=1:N) xx(K, 1:M) = yy(1:M, K)

! These statements both copy columns 1 through N of array Y into rows 1
! through N of array X. They are equivalent to

     xx(1:N, 1:M) = TRANSPOSE (yy(1:M, 1:N) )

! Example 2:
! The following FORALL statement computes five partial sums of subarrays of J.

     jj = (/ 1, 2, 3, 4, 5 /)

     FORALL (K = 1:5) jj(K) = SUM (jj(1:K) )

! SUM is allowed in a FORALL because intrinsic functions are pure (12.7). After
! execution of the FORALL statement, J = (/ 1, 3, 6, 10, 15 /).

!     Example 3:

     FORALL (I = 2:N-1) X(I) = (X(I-1) + 2*X(I) + X(I+1) ) / 4

!     has the same effect as

     X(2:N-1) = (X(1:N-2) + 2*X(2:N-1) + X(3:N) ) / 4

end

# Program Execution:

A make file complies all the algorithms and to run the program we use:  
`./ConvexHull`  
Then it shows two options;  
Please select input file [1] or random generation [2]:  
If “1” is selected it asks “Please input file name”:  
Please input file name: set10.dat  
Then the output will be generated as set10.out.  
If “2” is selected is asks “Please input points number”:  
Please input points number: 50  
Then the output will be generated as set50.out.

# Motivation and Background:

The convex hull of a set of points is characterized as the smallest convex polygon, that encases the entirety of the points in the set. Convex implies, that the polygon has no corner that is bowed inwards. We can solve problems like Tracking disease Epidemic, Nuclear Leak Evacuation using convex hull. Also, not necessarily a best solution but we can use convex hull in image processing.

# Pseudocode and Implementation:

## Jarvis March: (Gift Wrapping Algorithm):

```
algorithm jarvis(S) is
    // S is the set of points
    // P will be the set of points which form the convex hull. Final set size is i.
    pointOnHull = leftmost point in S // which is guaranteed to be part of the CH(S)
    i := 0
    repeat
        P[i] := pointOnHull
        endpoint := S[0] // initial endpoint for a candidate edge on the hull
        for j from 0 to |S| do
            // endpoint == pointOnHull is a rare case and can happen only when j == 1 and a better endpoint has not yet been set for the loop
            if (endpoint == pointOnHull) or (S[j] is on left of line from P[i] to endpoint) then
                endpoint := S[j] // found greater left turn, update endpoint
        i := i + 1
        pointOnHull = endpoint
    until endpoint = P[0] // wrapped around to first hull point
```

## Quick Hull:

```
Input = a set S of n points
Assume that there are at least 2 points in the input set S of points
QuickHull (S)
{
    // Find convex hull from the set S of n points
    Convex_Hull := {}
    Find left and right most points, say A & B
    Add A & B to convex hull
    Segment AB divides the remaining (n-2) points into 2 groups S1 and S2
        where S1 are points in S that are on the right side of the oriented line from A to B, and
            S2 are points in S that are on the right side of the oriented line from B to A
    FindHull (S1, A, B)
    FindHull (S2, B, A)
}
FindHull (Sk, P, Q)
{
    // Find points on convex hull from the set Sk of points
    // that are on the right side of the oriented line from P to Q
    If Sk has no point, then return.
    From the given set of points in Sk, find farthest point, say C, from segment PQ
    Add point C to convex hull at the location between P and Q .
    Three points P, Q and C partition the remaining points of Sk into 3 subsets: S0, S1, and S2
        where S0 are points inside triangle PCQ,
            S1 are points on the right side of the oriented line from P to C
            S2 are points on the right side of the oriented line from C to Q
    FindHull(S1, P, C)
    FindHull(S2, C, Q)
}
Output = Convex Hull
```

## Merge Hull:

```
MergeHull(points P)
    Sort points P according to x
    Return hull(P)
Hull(points P)
    If |P| <= 3 then
        Compute CH by brute force,
        Return
Partition P into two sets L and R ( with lower & higher coords x)
Recursively compute HL = hull(L), HR = hull(R)
H = Merge hulls (HL , HR) by computing
    Upper_tangent (HL , HR) // find nearest points, HL CCW , HR CW
    Lower_tangent (HL , HR) // (HL CW, HR CCW)
    Discard points between these two tangents
Return H
**For Upper Tangent:**
Upper_tangent (HL , HR)
L <- line joining the rightmost point of a and leftmost point of b.
while (L crosses any of the polygons)
{
    while(L crosses b)
        L <- L' : the point on b moves up.
    while(L crosses a)
        L <- L' : the point on a moves up.
}
**For Lower Tangent:**
Lower_tangent (HL , HR)
    L <- line joining the rightmost point of a and leftmost point of b.
    while (L crosses any of the polygons)
    {
        while (L crosses b)
            L <- L' : the point on b moves down.
        while (L crosses a)
            L <- L' : the point on a moves down.
    }

```

1.	Layered Tree Traversal
	Write a layered tree traversal C program to do tree traversal as defined below.

1) The traversal starts at the Root;
2) The nodes are visited from higher layers (close to Root) to low layers in order;
3) At each layer, the nodes are visited from right to left.

The tree is:
Root = 1, Left(1) = -2, Right(1) = -3;
Left(-2) = 4, Right(-2) = 5;
Left(-3) = 6, Right(-3)= 7;
Left(5) = -8, Right(5)= -9;
Left(7) = 10, Right(7) = 11;
Left(11) = -12, Right(11) = -13;
Left(-13) = 14.
You program should output the printed sequence of node IDs (positive or negative)
according to the required order, along with the level and the sequence number within the same level, and the sum of values in each level. The root level is labeled with a value 0. Recursion is useless for this program. You have to learn how to make use of the queue data structure to write the program. For example, for the partial tree 
Root = 1, Left(1) = -2, Right(1) = -3, the required print-out is:

level 0, (seq_num=1, value=1), sum = 1.
level 1, (seq_num=1, value=-3), (seq_num=2, value=-2), sum = -5.
…….

/*
 * Implementation of a stack test program for the first task
 * in the "Datastructures and algorithms" course.
 *
 * Author: Adam Pettersson (hed21apn)
 *
 * Version information:
 *	 2022-04-12: v1.0, first public version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stack.h"

void test_stack_empty(void);
void test_stack_is_empty(void);
void test_stack_push(void);
void test_stack_pop(void);
void test_stack_top(void);
bool value_equal(int v1, int v2);
void test_stack_push_pop_different_elements(void);
void test_stack_insert_remove_beginning(void);
void test_stack_reach_max_size(void);
void test_stack_pop_empty(void);

int main(void)
{

	test_stack_empty();
	test_stack_is_empty();
	test_stack_push();
	test_stack_pop();
	test_stack_top();
	test_stack_push_pop_different_elements();
	test_stack_insert_remove_beginning();
	test_stack_reach_max_size();
	test_stack_pop_empty();
	fprintf(stderr, "SUCCESS: Implementation passed all tests. Normal exit.\n");

	return 0;
}

/*
 * value_equal() - Compare two integer values for equality.
 * @v1: The first integer value to compare.
 * @v2: The second integer value to compare.
 *
 * Return: True if the two integer values are equal, false otherwise.
 */
bool value_equal(int v1, int v2)
{
	return v1 == v2;
}

/*
 * test_stack_empty() - Test if an empty stack is created successfully.
 * Preconditions: stack_empty() and stack_is_empty() work correctly
 */
void test_stack_empty(void)
{

	fprintf(stderr, "Running test: test_stack_empty()");

	stack *s = stack_empty(NULL);

	if (!stack_is_empty(s))
	{
		fprintf(stderr, "FAIL: the stack_is_empty did not work as intended\n");
		exit(EXIT_FAILURE);
	}
	else
	{
		fprintf(stderr, "SUCCESS\n");
		stack_kill(s);
	}
}

/*
 * test_stack_is_empty() - Test if the stack is empty after being created.
 * Preconditions: stack_empty() and stack_top() work correctly
 */
void test_stack_is_empty(void)
{
	stack *s = stack_empty(NULL);
	fprintf(stderr, "Running test: test_stack_is_empty()");

	if (stack_is_empty(s))
	{
		fprintf(stderr, "SUCCESS\n");
		stack_kill(s);
	}
	else
	{
		fprintf(stderr, "FAIL: Expected stack to be empty, but it's not.\n");
		exit(EXIT_FAILURE);
	}
}

/*
 * test_stack_push() - Test if a value can be pushed onto the stack correctly.
 * Preconditions: stack_empty(), stack_push(), and stack_top() work correctly
 */
void test_stack_push(void)
{
	fprintf(stderr, "Running test: test_stack_push()\n");

	stack *s = stack_empty(NULL);
	int value = 42;
	s = stack_push(s, &value);

	if (value_equal(*(int *)stack_top(s), 42))
	{
		fprintf(stderr, "SUCCESS\n");
		stack_kill(s);
	}
	else
	{
		fprintf(stderr, "FAIL: The value was not pushed correctly.\n");
		exit(EXIT_FAILURE);
	}
}

/*
 * test_stack_pop() - Test if a value can be popped from the stack correctly.
 * Preconditions: stack_empty(), stack_push(), stack_pop(), and stack_top() work correctly
 */
void test_stack_pop(void)
{
	fprintf(stderr, "Running test: test_stack_pop()");

	stack *s = stack_empty(NULL);
	int value1 = 42;
	int value2 = 43;
	s = stack_push(s, &value1);
	s = stack_push(s, &value2);
	s = stack_pop(s);

	if (value_equal(*(int *)stack_top(s), 42))
	{
		fprintf(stderr, "SUCCESS\n");
		stack_kill(s);
	}
	else
	{
		fprintf(stderr, "FAIL: The value was not popped correctly.\n");
		exit(EXIT_FAILURE);
	}
}

/*
 * test_stack_top() - Test if the top value of the stack can be inspected correctly.
 * Preconditions: stack_empty(), stack_push(), and stack_top() work correctly
 */
void test_stack_top(void)
{
	fprintf(stderr, "Running test: test_stack_top()");

	stack *s = stack_empty(NULL);
	int value1 = 42;
	int value2 = 43;
	s = stack_push(s, &value1);
	s = stack_push(s, &value2);

	if (value_equal(*(int *)stack_top(s), 43))
	{
		fprintf(stderr, "SUCCESS\n");

		stack_kill(s);
	}
	else
	{
		fprintf(stderr, "FAIL: The top value was not inspected correctly.\n");
		exit(EXIT_FAILURE);
	}
}

/*
 * test_stack_push_pop_different_elements() - Test if pushing and popping different elements
 *     on the stack work correctly.
 * Preconditions: stack_empty(), stack_push(), stack_pop(), and stack_top() work correctly
 */
void test_stack_push_pop_different_elements(void)
{
	fprintf(stderr, "Running test: test_stack_push_pop_different_elements()");

	stack *s = stack_empty(NULL);

	int value1 = 42;
	int value2 = 99;
	int value3 = 7;

	s = stack_push(s, &value1);
	s = stack_push(s, &value2);
	s = stack_push(s, &value3);

	int third_element = *(int *)stack_top(s);
	s = stack_pop(s);

	int second_element = *(int *)stack_top(s);
	s = stack_pop(s);

	int first_element = *(int *)stack_top(s);
	s = stack_pop(s);

	if (first_element == 42 && second_element == 99 && third_element == 7)
	{
		fprintf(stderr, "SUCCESS");
	}
	else
	{
		fprintf(stderr, "FAIL: Different elements were not pushed and popped correctly.\n");
		exit(EXIT_FAILURE);
	}

	stack_kill(s);
}

/*
 * test_stack_insert_remove_beginning() - Test if inserting and removing elements at the beginning
 *     of the stack works correctly.
 * Preconditions: stack_empty(), stack_push(), stack_pop(), and stack_top() work correctly
 */
void test_stack_insert_remove_beginning(void)
{
	fprintf(stderr, "Running test: test_stack_insert_remove_beginning()\n");

	stack *s = stack_empty(NULL);
	int value1 = 42;
	int value2 = 99;

	s = stack_push(s, &value1);
	s = stack_push(s, &value2);

	s = stack_pop(s);

	if (value_equal(*(int *)stack_top(s), 42))
	{
		fprintf(stderr, "SUCCESS\n");
	}
	else
	{
		fprintf(stderr, "FAIL: Inserting and removing elements at the beginning doesn't work correctly.\n");
		exit(EXIT_FAILURE);
	}

	stack_kill(s);
}

/*
 * test_stack_reach_max_size() - Test if the stack behaves correctly when reaching its maximum size.
 * Preconditions: stack_empty(), stack_push(), stack_pop(), and stack_top() work correctly
 */
void test_stack_reach_max_size(void)
{
	fprintf(stderr, "Running test: test_stack_reach_max_size()");
	stack *s = stack_empty(NULL);
	const int max_size = 10;
	int values[max_size];

	for (int i = 0; i < max_size; i++)
	{
		values[i] = i;
		s = stack_push(s, &values[i]);
	}

	for (int i = max_size - 1; i >= 0; i--)
	{
		if (!value_equal(*(int *)stack_top(s), i))
		{
			fprintf(stderr, "FAIL: Stack doesn't behave correctly when reaching its maximum size.\n");
			exit(EXIT_FAILURE);
		}
		s = stack_pop(s);
	}

	fprintf(stderr, "SUCCESS\n");

	stack_kill(s);
}

/*
 * test_stack_pop_empty() - Test if the stack_pop() function behaves correctly when the stack is empty.
 * Preconditions: stack_empty(), stack_pop(), and stack_is_empty() work correctly
 */
void test_stack_pop_empty(void)
{
	fprintf(stderr, "Running test: test_stack_pop_empty()");

	stack *s = stack_empty(NULL);
	s = stack_pop(s);

	if (stack_is_empty(s))
	{
		fprintf(stderr, "SUCCESS\n");
		stack_kill(s);
	}
	else
	{
		fprintf(stderr, "FAIL: stack_pop does not handle empty stack correctly.\n");
		exit(EXIT_FAILURE);
	}
}

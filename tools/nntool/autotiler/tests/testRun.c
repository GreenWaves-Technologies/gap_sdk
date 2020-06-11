#include "Gap.h"
#include <stdio.h>
#include "testKernels.h"

#define SIZEHW 320 * 240
#define SIZE888 320 * 240 * 3

int err_count = 0;
unsigned short *In565;
unsigned char *In888, *In8;
signed char *Out8, *Out888;

#define assert(__test, __h, __w, __c, __got, __expect)                                        \
	while (__got != __expect)                                                                 \
	{                                                                                         \
		err_count++;                                                                          \
		printf(" %s[%d, %d, %d] expected %d got %d", __test, __h, __w, __c, __expect, __got); \
		return 1;                                                                             \
	}

typedef struct
{
	void (*test_func)(void *In, signed char *Out, int offset);
	void *In;
	signed char *Out;
	int offset;
} test_args_t;

typedef struct
{
	void (*test_func)(void *In, signed char *Out, int offset);
	void *In;
	signed char *Out;
} test_args_16_t;

void test565(void *In, signed char *Out, int offset)
{
	if (offset)
	{
		printf("testfunc1\n");
		testfunc1((unsigned short *)In, Out);
	}
	else
	{
		printf("testfunc2\n");
		testfunc2((unsigned short *)In, Out);
	}
}

void test888(void *In, signed char *Out, int offset)
{
	if (offset)
	{
		printf("testfunc3\n");
		testfunc3((unsigned char *)In, Out);
	}
	else
	{
		printf("testfunc4\n");
		testfunc4((unsigned char *)In, Out);
	}
}

void testBW(void *In, signed char *Out, int offset)
{
	if (offset)
	{
		printf("testfunc5\n");
		testfunc5((unsigned char *)In, Out);
	}
	else
	{
		printf("testfunc6\n");
		testfunc6((unsigned char *)In, Out);
	}
}

void prepare565(void *buf)
{
	unsigned short *sbuf = (unsigned short *)buf;
	printf("prepare565\n");
	for (int h = 0; h < 240; h++)
	{
		for (int w = 0; w < 320; w++)
		{
			sbuf[h * 320 + w] = ((h % 3) << 11) | (((h % 3) + 1) << 6) | ((h % 3) + 2); //testfunc1-2
		}
	}
}

void prepare888(void *buf)
{
	unsigned char *cbuf = (unsigned char *)buf;
	printf("prepare888\n");
	for (int h = 0; h < 240; h++)
	{
		for (int w = 0; w < 320; w++)
		{
			for (int c = 0; c < 3; c++)
			{
				cbuf[h * 320 * 3 + w * 3 + c] = h + c;
			}
		}
	}
}

void prepareBW(void *buf)
{
	unsigned char *cbuf = (unsigned char *)buf;
	printf("prepareBW\n");
	for (int h = 0; h < 240; h++)
	{
		for (int w = 0; w < 320; w++)
		{
			cbuf[h * 320 + w] = h;
		}
	}
}

int verify565(signed char *buf, int offset)
{
	printf("verify565 offset(%d)", offset);
	for (int c = 0; c < 3; c++)
	{
		for (int h = 0; h < 240; h++)
		{
			for (int w = 0; w < 320; w++)
			{
				if (offset)
				{
					assert("testfunc1", h, w, c, buf[c * 240 * 320 + 320 * h + w], (((h % 3) + c) << 3) - 128); //testfunc1
				}
				else
				{
					assert("testfunc2", h, w, c, buf[c * 240 * 320 + 320 * h + w], (((h % 3) + c) << 3) >> 1); //testfunc2
				}
			}
		}
	}
	return 0;
}

int verify888(signed char *buf, int offset)
{
	printf("verify888 offset(%d)", offset);
	for (int c = 0; c < 3; c++)
	{
		for (int h = 0; h < 240; h++)
		{
			for (int w = 0; w < 320; w++)
			{
				if (offset)
				{
					assert("testfunc3", h, w, c, buf[c * 240 * 320 + 320 * h + w], ((h + c) - 128)); //testfunc3
				}
				else
				{
					assert("testfunc4", h, w, c, buf[c * 240 * 320 + 320 * h + w], ((h + c) >> 1)); //testfunc4
				}
			}
		}
	}
	return 0;
}

int verifyBW(signed char *buf, int offset)
{
	printf("verifyBW offset(%d)", offset);
	for (int h = 0; h < 240; h++)
	{
		for (int w = 0; w < 320; w++)
		{
			if (offset)
			{
				assert("testfunc5", h, w, 1, buf[320 * h + w], h - 128); //testfunc5
			}
			else
			{
				assert("testfunc6", h, w, 1, buf[320 * h + w], h >> 1); //testfunc6
			}
		}
	}
	return 0;
}

void do_test_cluster(void *arg)
{
	test_args_t *targs = (test_args_t *)arg;
	targs->test_func(targs->In, targs->Out, targs->offset);
}

void do_test(void *pcluster_dev, void *In, signed char *Out,
			 void (*prepare)(void *),
			 int (*verify)(signed char *, int),
			 void (*test_func)(void *In, signed char *Out, int offset))
{
	for (int o = 1; o >= 0; o--)
	{
		prepare(In);
#ifndef __EMUL__
		struct pi_device *cluster_dev = (struct pi_device *)pcluster_dev;
		test_args_t *targs;
		targs = pmsis_l2_malloc(sizeof(test_args_t));
		memset(targs, 0, sizeof(test_args_t));
		targs->test_func = test_func;
		targs->In = In;
		targs->Out = Out;
		targs->offset = o;

		struct pi_cluster_task *task;
		task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
		memset(task, 0, sizeof(struct pi_cluster_task));
		task->entry = &do_test_cluster;
		task->stack_size = 4096;
		task->slave_stack_size = 1024;
		task->arg = targs;

		pi_cluster_send_task_to_cl(cluster_dev, task);

		pi_l2_free(task, sizeof(struct pi_cluster_task));
		pi_l2_free(targs, sizeof(test_args_t));
#else
		test_func(In, Out, o);
#endif
		if (verify(Out, o))
		{
			printf(" failed !!\n");
		}
		else
		{
			printf(" passed\n");
		}
	}
}

void testBW16(void *In, signed short int *Out)
{
	printf("testfunc7\n");
	testfunc7((unsigned char *)In, Out);
}

int verifyBW16(signed short int *buf)
{
	printf("verifyBW 16 bits");
	for (int h = 0; h < 240; h++)
	{
		for (int w = 0; w < 320; w++)
		{
			assert("testfunc7", h, w, 1, buf[320 * h + w], h << 7); //testfunc7
		}
	}
	return 0;
}

void testRGB16(void *In, signed short int *Out)
{
	printf("testfunc8\n");
	testfunc8((unsigned char *)In, Out);
}

int verifyRGB16(signed short int *buf)
{
	printf("verifyRGB16");
	for (int c = 0; c < 3; c++)
	{
		for (int h = 0; h < 240; h++)
		{
			for (int w = 0; w < 320; w++)
			{
				assert("testfunc8", h, w, c, buf[c * 240 * 320 + 320 * h + w], ((h + c) << 7)); //testfunc8
			}
		}
	}
	return 0;
}

void do_test16(void *pcluster_dev, void *In, signed short int *Out,
			   void (*prepare)(void *),
			   int (*verify)(signed short int *),
			   void (*test_func)(void *In, signed short int *Out))
{
	prepare(In);
#ifndef __EMUL__
	struct pi_device *cluster_dev = (struct pi_device *)pcluster_dev;
	test_args_16_t *targs;
	targs = pmsis_l2_malloc(sizeof(test_args_16_t));
	memset(targs, 0, sizeof(test_args_16_t));
	targs->test_func = test_func;
	targs->In = In;
	targs->Out = Out;

	struct pi_cluster_task *task;
	task = pmsis_l2_malloc(sizeof(struct pi_cluster_task));
	memset(task, 0, sizeof(struct pi_cluster_task));
	task->entry = &do_test_cluster;
	task->stack_size = 4096;
	task->slave_stack_size = 1024;
	task->arg = targs;

	pi_cluster_send_task_to_cl(cluster_dev, task);

	pi_l2_free(task, sizeof(struct pi_cluster_task));
	pi_l2_free(targs, sizeof(test_args_16_t));
#else
	test_func(In, Out);
#endif
	if (verify(Out))
	{
		printf(" failed !!\n");
	}
	else
	{
		printf(" passed\n");
	}
}

int start()
{

	unsigned char *In = AT_L2_ALLOC(0, SIZE888 * sizeof(unsigned char));
	if (!In)
	{
		printf("unable to allocate In\n");
		exit(1);
	}
	signed char *Out = AT_L2_ALLOC(0, SIZE888 * sizeof(signed char));
	if (!Out)
	{
		printf("unable to allocate Out\n");
		exit(1);
	}
	signed short int *Out16 = AT_L2_ALLOC(0, SIZE888 * sizeof(signed short int));
	if (!Out16)
	{
		printf("unable to allocate Out16\n");
		exit(1);
	}

	void *pcluster_dev;
#ifndef __EMUL__
	struct pi_device cluster_dev;
	struct pi_cluster_conf conf;
	pi_cluster_conf_init(&conf);
	conf.id = 0; /* Set cluster ID. */
	pi_open_from_conf(&cluster_dev, (void *)&conf);
	pi_cluster_open(&cluster_dev);
	pi_freq_set(PI_FREQ_DOMAIN_CL, 50000000);
	pi_freq_set(PI_FREQ_DOMAIN_FC, 50000000);
	pcluster_dev = &cluster_dev;
#else
	pcluster_dev = 0;
#endif
	L1_Memory = (AT_L1_POINTER)AT_L1_ALLOC(0, _L1_Memory_SIZE * sizeof(signed char));
	if (!L1_Memory)
	{
		printf("unable to allocate L1");
		exit(1);
	}
	do_test(pcluster_dev, In, Out, prepare565, verify565, test565);
	do_test(pcluster_dev, In, Out, prepare888, verify888, test888);
	do_test(pcluster_dev, In, Out, prepareBW, verifyBW, testBW);
	do_test16(pcluster_dev, In, Out16, prepareBW, verifyBW16, testBW16);
	do_test16(pcluster_dev, In, Out16, prepare888, verifyRGB16, testRGB16);

	printf("test %s\n", (err_count ? "failed" : "passed"));

#ifdef __EMUL__
	return err_count;
#else
	pi_cluster_close(&cluster_dev);
	pmsis_exit(err_count);
#endif
}
int main(void)
{
#ifndef __EMUL__
	return pmsis_kickoff((void *)start);
#else
	return start();
#endif
}

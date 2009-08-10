#include <iostream>
#include <cstdio>

#include <pthread.h>

typedef struct builder 
{
	builder() : arch(""), retval(1) { }

	std::string arch;
	int retval;
} builder_t;

void *build_helper(void *data)
{
	builder_t *fdd = (builder_t*)data;

	std::cout << "\tstarting build for " << fdd->arch << std::endl;

	usleep(1000000 + rand() % 1000000);

	std::string cmd = "./build.sh --client=s25client --arch=" + fdd->arch;

	fdd->retval = system(cmd.c_str());

	return (void*)&fdd->retval;
}

void *packer_helper(void *data)
{
	builder_t *fdd = (builder_t*)data;

	std::cout << "\tstarting packing for " << fdd->arch << std::endl;

	usleep(1000000 + rand() % 1000000);

	std::string cmd = "./packager.sh --client=s25client --arch=" + fdd->arch;

	fdd->retval = system(cmd.c_str());

	return (void*)&fdd->retval;
}

void *upload_helper(void *data)
{
	builder_t *fdd = (builder_t*)data;

	std::cout << "\tstarting upload for " << fdd->arch << std::endl;

	usleep(1000000 + rand() % 1000000);

	std::string cmd = "./upload.sh --client=s25client --arch=" + fdd->arch;

	fdd->retval = system(cmd.c_str());

	return (void*)&fdd->retval;
}

int main(void)
{
	srand(534534);

	pthread_t tid1, tid2, tid3, tid4;
	builder_t fdd1, fdd2, fdd3, fdd4;
	int *ret1 = &fdd1.retval, *ret2 = &fdd2.retval, *ret3 = &fdd3.retval, *ret4 = &fdd4.retval;
	std::string cmd;
	int retval = 1;
	
	fdd1.arch = "i686";
	fdd2.arch = "x86_64";
	fdd3.arch = "win32";
	fdd4.arch = "macos";

	/*std::cout << "updating source tree" << std::endl;
	cmd = "./update.sh --client=s25client";
	retval = system(cmd.c_str());
	std::cout << "update done: " << retval << std::endl;

	if(retval != 0)
	{
		std::cout << "update had errors, do not build anything" << std::endl;
		return 2;
	}

	std::cout << "starting build threads" << std::endl;

	if (pthread_create(&tid1, NULL, build_helper, (void *)&fdd1) == -1)
		return 1;
	pthread_join(tid1, (void **)&ret1);

	if(*ret1 == 0)
	{
		if (pthread_create(&tid2, NULL, build_helper, (void *)&fdd2) == -1)
			return 1;
		pthread_join(tid2, (void **)&ret2);
		if(*ret2 == 0)
		{
			if (pthread_create(&tid3, NULL, build_helper, (void *)&fdd3) == -1)
				return 1;
			pthread_join(tid3, (void **)&ret3);

			if(*ret3 == 0)
			{
				if (pthread_create(&tid4, NULL, build_helper, (void *)&fdd4) == -1)
					return 1;
				pthread_join(tid4, (void **)&ret4);
			}
		}
	}

	std::cout << "build done: " << *ret1 << " " << *ret2 << " " << *ret3 << " " << *ret4 << std::endl;

	if(*ret1 != 0 || *ret2 != 0 || *ret3 != 0 || *ret4 != 0)
	{
		std::cout << "build had errors, do not package anything" << std::endl;
		return 2;
	}*/

	std::cout << "starting packer threads" << std::endl;
	if (pthread_create(&tid1, NULL, packer_helper, (void *)&fdd1) == -1)
		return 1;
	if (pthread_create(&tid2, NULL, packer_helper, (void *)&fdd2) == -1)
		return 1;
	if (pthread_create(&tid3, NULL, packer_helper, (void *)&fdd3) == -1)
		return 1;
	if (pthread_create(&tid4, NULL, packer_helper, (void *)&fdd4) == -1)
		return 1;

	pthread_join(tid1, (void **)&ret1);
	pthread_join(tid2, (void **)&ret2);
	pthread_join(tid3, (void **)&ret3);
	pthread_join(tid4, (void **)&ret4);

	std::cout << "packing done: " << *ret1 << " " << *ret2 << " " << *ret3 << " " << *ret4 << std::endl;

	if(*ret1 != 0 || *ret2 != 0 || *ret3 != 0 || *ret4 != 0)
	{
		std::cout << "packing had errors, do not upload anything" << std::endl;
		return 2;
	}

	std::cout << "starting upload threads" << std::endl;
	if (pthread_create(&tid1, NULL, upload_helper, (void *)&fdd1) == -1)
		return 1;
	if (pthread_create(&tid2, NULL, upload_helper, (void *)&fdd2) == -1)
		return 1;
	if (pthread_create(&tid3, NULL, upload_helper, (void *)&fdd3) == -1)
		return 1;
	if (pthread_create(&tid4, NULL, upload_helper, (void *)&fdd4) == -1)
		return 1;

	pthread_join(tid1, (void **)&ret1);
	pthread_join(tid2, (void **)&ret2);
	pthread_join(tid3, (void **)&ret3);
	pthread_join(tid4, (void **)&ret4);

	std::cout << "upload done: " << *ret1 << " " << *ret2 << " " << *ret3 << " " << *ret4 << std::endl;

	if(*ret1 != 0 || *ret2 != 0 || *ret3 != 0 || *ret4 != 0)
	{
		std::cout << "upload had errors, do not send message" << std::endl;
		return 2;
	}

	std::cout << "sending message" << std::endl;

	cmd = "./message.sh";
	retval = system(cmd.c_str());

	std::cout << "message sent: " << retval << std::endl;

	return 0;
}

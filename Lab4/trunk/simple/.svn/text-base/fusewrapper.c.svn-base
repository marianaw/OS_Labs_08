/* $Date: 2008-11-05 16:17:59 -0200 (Wed, 05 Nov 2008) $ */
/* $Revision: 698 $ */

#define _GNU_SOURCE
#define FUSE_USE_VERSION 26

#include <assert.h>

#include <stdlib.h>
#include <fuse.h>
#include <errno.h>
#include <malloc.h>
#include <time.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "simple.h"
#include "log.h"

/* Variable global que contiene el SimpleFS */
simple *c = NULL;

/*
 * Operaciones Básicas
 */

static int c_getattr(const char *path, struct stat *stbuf)
{
	int rta = 0;
	debug3("[%li] ",time(NULL));
	debug1("getattr:%s",path);
	rta = simple_getattr(c,path,stbuf);
	debug2(" -> mode:%o size:%li",stbuf->st_mode,(long)stbuf->st_size);
	debug3(" returns:%i",-rta);
	debug1("\n");
	return -rta;
}

static int c_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int rta = 0;
	debug3("[%li] ",time(NULL));
	debug1("mknod:%s mode:%o",path,mode);
	rta = simple_mknod(c,path,mode);
	debug3(" -> returns:%i",-rta);
	debug1("\n");
	return -rta;
}

static int c_unlink(const char *path)
{
	int rta = 0;
	debug3("[%li] ",time(NULL));
	debug1("unlink_file:%s",path);
	rta = simple_unlink(c,path);
	debug3(" -> returns:%i",-rta);
	debug1("\n");
	return -rta;
}

static int c_rename(const char *from, const char *to)
{
	int rta = 0;
	debug3("[%li] ",time(NULL));
	debug1("rename from:%s to:%s",from,to);
	rta = simple_rename(c,from,to);
	debug3(" -> returns:%i",-rta);
	debug1("\n");
	return -rta;
}

static int c_chmod(const char *path, mode_t mode)
{
	int rta = 0;
	debug3("[%li] ",time(NULL));
	debug1("chmod:%s modo:%o", path, mode);
	rta = simple_chmod(c,path,mode);
	debug3(" -> returns:%i",-rta);
	debug1("\n");
	return -rta;
}

static int c_chown(const char *path, uid_t uid, gid_t gid)
{
	int rta = 0;
	debug3("[%li] ",time(NULL));
	debug1("chown:%s uid:%i gid:%i",path,uid,gid);
	rta = simple_chown(c,path,uid,gid);
	debug3(" -> returns:%i",-rta);
	debug1("\n");
	return -rta;
}

static int c_truncate(const char *path, off_t size)
{
	int rta = 0;
	debug3("[%li] ",time(NULL));
	debug1("*truncate:%s size:%li",path,(long)size);
	debug1("\n");
	return rta;
}

static int c_utime(const char *path, struct utimbuf *utb)
{
	int rta = 0;
	struct timespec tv[2];
	tv[0].tv_sec = utb->actime;
	tv[0].tv_nsec = 0;
	tv[1].tv_sec = utb->modtime;
	tv[1].tv_nsec = 0;
	debug3("[%li] ",time(NULL));
	debug1("utime:%s atime:%li mtime:%li", path, (long)utb->actime, (long)utb->modtime);
	rta = simple_utimens(c,path,tv);
	debug3(" -> returns:%i",-rta);
	debug1("\n");
	return -rta;
}

static int c_open(const char *path, struct fuse_file_info *fi)
{
	int rta = 0;
	debug3("[%li] ",time(NULL));
	debug1("*open:%s flags:%i", path, fi->flags);
	debug1("\n");
	return rta;
}

static int c_read(const char *path, char *buf, size_t size, off_t offset,
		  struct fuse_file_info *fi)
{
	int rta = 0;
	(void) fi;
	debug3("[%li] ",time(NULL));
	debug1("read:%s size:%li offset:%li",path,(long)size,(long)offset);
	rta = simple_read(c,path,buf,size,offset);
	debug2(" -> returns:%i",rta);
	debug1("\n");
	return rta;
}

static int c_write(const char *path, const char *buf, size_t size,off_t offset,
		   struct fuse_file_info *fi)
{
	int rta = 0;
	(void) fi;
	debug3("[%li] ",time(NULL));
	debug1("write:%s size:%li offset:%li",path,(long)size,(long)offset);
	rta = simple_write(c,path,buf,size,offset);
	debug2(" -> returns:%i",rta);
	debug1("\n");
	return rta;
}

static int c_release(const char *path, struct fuse_file_info *fi)
{
	int rta = 0;
	debug3("[%li] ",time(NULL));
	debug1("*release:%s flags:%i",path,fi->flags);
	debug1("\n");
	return rta;
}

static int c_utimens(const char *path, const struct timespec tv[2])
{
	int rta = 0;
	debug3("[%li] ",time(NULL));
	debug1("utimens:%s atime:%li mtime:%li",path,(long)tv[0].tv_sec,(long)tv[1].tv_sec);
	rta = simple_utimens(c,path,tv);
	debug3(" -> returns:%i",-rta);
	debug1("\n");
	return -rta;
}

/*
 * Operaciones sobre Directorios
 */

static int c_mkdir(const char *path, mode_t mode)
{
	int rta =  0;
	debug3("[%li] ",time(NULL));
	debug1("mkdir:%s mode:%o",path,mode);
	rta = simple_mkdir(c,path,mode);
	debug3(" -> returns:%i",-rta);
	debug1("\n");
	return -rta;
}

static int c_rmdir(const char *path)
{
	int rta = 0;
	debug3("[%li] ",time(NULL));
	debug1("rmdir:%s",path);
	rta = simple_rmdir(c,path);
	debug3(" -> returns:%i",-rta);
	debug1("\n");
	return -rta;
}

static int c_readdir(const char *path, void *buf,
		     fuse_fill_dir_t filler, off_t offset,
		     struct fuse_file_info *fi)
{
	int rta = 0;
	(void) fi; /* Para que no diga que la variable está sin usar */
	debug3("[%li] ",time(NULL));
	debug1("readdir:%s",path);
	rta = simple_readdir(c, path, buf, filler);
	debug3(" -> returns:%i",-rta);
	debug1("\n");
	return -rta;
}


static struct fuse_operations c_oper = {
	/* Primera parte: operaciones básicas, NI = no implementada */
	.getattr  = c_getattr,
	.mknod	  = c_mknod,
	.unlink	  = c_unlink,
	.rename	  = c_rename,
	.chmod	  = c_chmod,
	.chown	  = c_chown,
	.truncate = c_truncate,	/* NI */
	.utime    = c_utime,
	.open	  = c_open,	/* NI */
	.read	  = c_read,
	.write	  = c_write,
	.release  = c_release,	/* NI */
	.utimens  = c_utimens,
	/* Segunda parte: operaciones sobre directorios */
	.mkdir	  = c_mkdir,
	.rmdir	  = c_rmdir,
	.readdir  = c_readdir,
};

/* !! end rafa !! */

int main(int argc, char *argv[])
{
	/* Cheat es una variable al estilo de argv pero solo con argumentos parseados como validos */
	/* La uso para trabajar menos*/
	char *cheat[10]; /* 10 es un indice que nunca se alcanza porque noy hay tantos argumentos validos */
	int i = 1;
	int argi = 1;
	char *logfilename = NULL;
	char *mountpoint = NULL;
	char *tailptr = NULL; /* Variable temporal usada pora strtol */
	int loglevel = 3; /* default loglevel */
	c_oper.utimens = 0;
	cheat[0] = argv[0];

	/* i==1 && argi == 1   i se usa con cheat y argi con argv */
	/* cuando consumo un argumento aumento argi */
	/* cuando agrego uno para fusemain aumento i */
	while(argi < argc)
	{
		if(strcmp(argv[argi],"-h") == 0)
		{
			printf("uso: %s [opciones] mountpoint\n"
				"opciones:\n"
				"\t-h\t\tImprime esta ayuda.\n\n"
				"\t-logfile file\n"
				"\t-l file\t\tGuarda los logs en el archivo especificado.\n\n"
				"\t-loglevel num\n"
				"\t-n num\t\tEspecifica el nivel de log. Por defecto es 3.\n\n"
				,argv[0]);
			return 1;
		}
		else if(strcmp(argv[argi],"-logfile") == 0
			|| strcmp(argv[argi],"-l") == 0)
		{
			argi++;
			if(argi < argc) logfilename = argv[argi];
			else
			{
				fprintf(stderr,"Se debe especificar un nombre"
					      " de archivo\n");
				return 1;
			}
			loglevel=1; /* Para evitar malos usos */
		}
		else if(strcmp(argv[argi],"-loglevel") == 0
			|| strcmp(argv[argi],"-n") == 0)
		{
			argi++;
			if(argi < argc)
			{
				loglevel = (int)strtol(argv[argi],&tailptr,10);
				if(tailptr == argv[argi])
				{
					fprintf(stderr,
						"El nivel de log debe"
						"ser ún numero\n");
					return 1;
				}
				if(loglevel < 0)
				{
					fprintf(stderr,
						"El nivel de log debe"
						"ser un número natural\n");
					return 1;
				}
			}
			else
			{
				fprintf(stderr,"Se debe especificar un nivel"
					      " de log\n");
				return 1;
			}
		} else if(argv[argi][0] == '-')
		{
				fprintf(stderr,"Opción no reconocida:"
					      "%s\n",argv[argi]);
				return 1;
		} else if(mountpoint == NULL)
		{
			mountpoint = argv[argi];
		}
		argi++;
	}
	if(mountpoint == NULL){
		fprintf(stderr,"Argumentos insuficientes: hace falta un mountpoint\n");
		return 1;
	}
	cheat[i++] = mountpoint;
	if(loglevel != 0) cheat[i++] = "-f";
	cheat[i] = NULL;

	if(log_open(logfilename,loglevel)!=0)
		fprintf(stderr,"Advertencia! no se pudo inicializar el log\n");
	
	debug3("Los numeros a la izquierda indican el momento en que la "
	       "llamada a la\nfunción se efectuó según time(NULL).\n");

	c = simple_create();
	if(c == NULL) return ENOMEM;
	
	fuse_main(i, cheat, &c_oper,NULL); /* El corazón del programa */
	
	simple_destroy(c);
	log_close();

	return 0;
}

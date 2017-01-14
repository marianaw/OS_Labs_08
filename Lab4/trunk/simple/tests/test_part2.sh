#!/bin/bash

# $Date: 2008-11-07 14:40:52 -0200 (Fri, 07 Nov 2008) $
# $Revision: 700 $

# Usé "Advanced Bash-Scripting Guide" http://tldp.org/LDP/abs/html/

TIMES=128
A_DIR="DIR.tmp"

die()
{
	echo "Falló!"
	exit 1
}


# Seccion: mkdir, rmdir
echo "#Pruebas básicas: mkdir, rmdir"

echo "  Crear $A_DIR"
mkdir $A_DIR || die

echo "  $A_DIR está"
ls $A_DIR >/dev/null || die

echo "  Borrar $A_DIR"
rmdir $A_DIR || die

echo "  Crear $A_DIR"
mkdir $A_DIR || die

echo "  Copiar $0 a $A_DIR/$0.tmp"
cp $0 $A_DIR/$0.tmp || die

echo "  $0.tmp no está en el root"
ls $0.tmp >/dev/null 2>/dev/null && die

echo "  $0.tmp está en el $A_DIR"
ls $A_DIR/$0.tmp >/dev/null || die

echo "  Borramos $A_DIR/$0.tmp, $A_DIR"
rm $A_DIR/$0.tmp || die
rmdir $A_DIR || die

echo "  Crear directorios [0,$TIMES), y en cada uno copiar $0, además de comprobar que es el mismo"
i=0
while [ "$i" -lt "$TIMES" ]
do
	mkdir $i || die
	cp $0 $i/ || die
	diff $0 $i/$0 || die
	let i=i+1
done

echo "  Borramos todos los directorios [0,$TIMES) recursivamante con rm -R"
i=0
while [ "$i" -lt "$TIMES" ]
do
	rm -R $i || die
	let i=i+1
done

echo "  Cavamos profundo en la estructura de directorios, dejando $0 en cada uno"
deep=""
i=0
while [ "$i" -lt "$TIMES" ]
do
	deep=$A_DIR/$deep
	mkdir $deep || die
	cp $0 $deep/$i
	let i=i+1
done

echo "  Comprobamos que $0 esté en cada uno de los directorios"
deep=""
i=0
while [ "$i" -lt "$TIMES" ]
do
	deep=$A_DIR/$deep
	diff $0 $deep/$i
	let i=i+1
done

echo "  Borramos $A_DIR recursivamente"
rm -R $A_DIR || die

# Con este test salta un bug que metí en readdir
echo "  Creamos directorios 1, 11 y dentro de cada uno un archivo 1, 11"
mkdir 1 11 || die
touch 1/1 11/11 || die

echo "  Comprobamos que solo esté donde corresponde"
ls 1/1 >/dev/null || die
ls 11/11 >/dev/null || die
ls 11/1 >/dev/null 2>/dev/null && die
ls 1/11 >/dev/null 2>/dev/null && die

echo "  Borramos 1 y 11"
rm -R 1/ 11/ || die



# Sección: misceláneas
echo "#Pruebas misceláneas"

echo "  Bajar linux-1.0.tar.bz2"
wget http://www.kernel.org/pub/linux/kernel/v1.0/linux-1.0.tar.bz2 >/dev/null 2>/dev/null || die

echo "  Descomprimir linux-1.0.tar.bz2"
tar jxf linux-1.0.tar.bz2 >/dev/null 2>/dev/null || die

echo "  Está linux/fs/ext/freelists.c y no está linux/fs/ext/bitmap.c"
ls linux/fs/ext/ | grep "freelists\.c" >/dev/null || die
ls linux/fs/ext/ | grep "bitmap\.c" >/dev/null 2>/dev/null && die

echo "  Borramos recursivamente todo el árbol linux/"
rm -R linux/ || die

echo "  Borramos linux-1.0.tar.bz2"
rm linux-1.0.tar.bz2 || die


#########################

# Sección final
echo "#Pruebas finalizadas al 100%"
exit 0

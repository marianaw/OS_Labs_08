#!/bin/bash

# $Date: 2008-11-07 19:55:08 -0200 (Fri, 07 Nov 2008) $
# $Revision: 701 $

# Usé "Advanced Bash-Scripting Guide" http://tldp.org/LDP/abs/html/

TIMES=128

die()
{
	echo "Falló!"
	exit 1
}


# Seccion: getattr, mknod, unlink, rename.
echo "#Pruebas básicas: getattr, mknod, unlink, rename"

echo "  Crear $0.tmp"
touch $0.tmp || die

echo "  $0.tmp está"
ls $0.tmp >/dev/null || die

echo "  Cambiamos el nombre de $0.tmp a $0.tmp.tmp"
mv $0.tmp $0.tmp.tmp || die

echo "  $0.tmp no está"
ls $0.tmp >/dev/null 2>/dev/null && die

echo "  Borrar el archivo $0.tmp.tmp"
rm $0.tmp.tmp || die

echo "  $0.tmp.tmp no está"
ls $0.tmp.tmp >/dev/null 2>/dev/null && die


echo "  Crear archivos [0,$TIMES)"
i=0
while [ "$i" -lt "$TIMES" ]
do
	touch $i || die
	let i=i+1
done

echo "  Ver que hay archivos [0,$TIMES)"
i=0
while [ "$i" -lt "$TIMES" ]
do
	ls $i >/dev/null || die
	let i=i+1
done

echo "  Borrar archivos [0,$TIMES)"
i=0
while [ "$i" -lt "$TIMES" ]
do
	rm $i || die
	let i=i+1
done


# Seccion: read, write.
echo "#Pruebas de consistencia read, write"

echo "  Copiar el kernel de Linux `uname -r`"
cp /boot/vmlinuz-`uname -r` . || die

echo "  Comparar el original con la copia"
diff /boot/vmlinuz-`uname -r` vmlinuz-`uname -r`
# No lo borro.

echo "  Bajar linux-1.0.tar.bz2"
wget http://www.kernel.org/pub/linux/kernel/v1.0/linux-1.0.tar.bz2 >/dev/null 2>/dev/null || die

echo "  Descompresión"
bunzip2 linux-1.0.tar.bz2 || die
# No borro ni el descomprimido


# Sección: metadatos
echo "#Pruebas de consistencia de metadatos"

echo "  Creamos $0.tmp"
cp $0 $0.tmp || die

echo "  El dueño no es el uid/gid"
uid=`stat -c "%u" $0.tmp`
gid=`stat -c "%g" $0.tmp`
if [ "$uid" != "$UID" ] || [ "$gid" != "$GROUPS" ]; then
	die
fi

echo "  Los tiempos no son 0"
if [ `stat -c "%X" $0.tmp` = 0 ] || [ `stat -c "%Y" $0.tmp` = 0 ] || [ `stat -c "%Z" $0.tmp` = 0 ]; then
	die
fi

echo "  Si toco $0.tmp y espero 1 segundo, el tiempo de Modificación, Acceso y Cambio avanzan"
atime=`stat -c "%X" $0.tmp`
mtime=`stat -c "%Y" $0.tmp`
ctime=`stat -c "%Z" $0.tmp`
sleep 1
touch $0.tmp
if [ `stat -c "%X" $0.tmp` -le "$atime" ] || [ `stat -c "%Y" $0.tmp` -le "$mtime" ] || [ `stat -c "%Z" $0.tmp` -le "$ctime" ]; then
	die
fi

echo "  Si leo $0.tmp el tiempo de Acceso avanza"
#falsos negativos con mount -o noatime
atime=`stat -c "%X" $0.tmp`
sleep 1
cat $0.tmp >/dev/null
if [ `stat -c "%X" $0.tmp` -le "$atime" ]; then
	die
fi
# Lo borro, ya no lo necesito más
rm $0.tmp

#TODO: hacer chown, chmod y ver que ctime avanza


# Sección: misceláneas
echo "#Pruebas misceláneas"

echo "  Borro vmlinuz-`uname -r`"
rm vmlinuz-`uname -r` || die

echo "  Copio linux-1.0.tar a vmlinuz-`uname -r`"
#TODO: copiar un archivo que no termine potencialmente en ceros
#hasta que no se implemente truncate(), usar esta opción (gracias FedeH y CharlyB)
cp --sparse=never linux-1.0.tar vmlinuz-`uname -r` || die

echo "  Comparo linux-1.0.tar a vmlinuz-`uname -r`"
diff linux-1.0.tar vmlinuz-`uname -r` || die

# Sección final
echo "#Pruebas finalizadas al 100%"
exit 0

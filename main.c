#include <stdio.h>
#include <conio.h>
#include <time.h>

typedef struct
{
    int codigo;
    char descripcion[11];
    float precio;
    int cantidad;
} tproductos;

typedef struct
{
    int id;
    char fecha[11];
    char nombre[11];
    tproductos listProduct[100];
    float total;
} venta;

int fecha(char f[11])
{
    time_t t = time(NULL);
    struct tm tiempoLocal = *localtime(&t);
    char fechaHora[11];
    char *formato = "%d/%m/%Y";
    int bytesEscritos =
        strftime(fechaHora, sizeof fechaHora, formato, &tiempoLocal);
//  if (bytesEscritos != 0) {
//    printf("Fecha y hora: %s", fechaHora);
//  } else {
//    printf("Error formateando fecha");
//  }
    strcpy(f, fechaHora);
}

void continuar()
{
    printf("Presione una tecla para continuar\n\n");
    getch();
}

int idProducto()
{
    int x;
    FILE *productos;
    productos = fopen ("productos.bin", "rb");
    if (productos==NULL) return 0;

    tproductos producto;

    while(!feof(productos))
    {
        fread(&producto,sizeof(tproductos),1,productos);
    }

    if(producto.codigo<0 || producto.codigo>100)
    {
        x = 1;
    }
    else
    {
        x = producto.codigo;
        x++;
    }
    fclose(productos);
    return (x);
}

void crearProducto()
{
    system("cls");
    FILE *productos;
    productos = fopen ("productos.bin", "ab");
    if (productos==NULL)
        return 0;

    tproductos producto;

    int x;
    x = idProducto(&x);
    producto.codigo = x;
    fflush(stdin);
    printf("Ingresa el nombre del producto: ");
    gets(&producto.descripcion);
    printf("Ingresa el precio: ");
    scanf("%f", &producto.precio);
    fflush(stdin);
    printf("Ingresa la cantidad:");
    scanf("%d", &producto.cantidad);

    fwrite(&producto,sizeof(tproductos),1,productos);

    continuar();
    fclose(productos);
    system("cls");
    return 0;
}

void listaProductos()
{
    system("cls");
    FILE *productos;
    productos = fopen ("productos.bin", "rb");
    if (productos==NULL)
    {
        system("cls");
        printf("No hay productos en tu inventario :(\n\n\n");
        continuar();
        system("cls");
        return 0;
    }
    tproductos producto;

    fread(&producto,sizeof(tproductos),1,productos);

    printf(" =========================== \n          PRODUCTOS\n =========================== \n");

    while(!feof(productos))
    {
        printf("   %d --> %s %.f %d\n",producto.codigo,producto.descripcion,producto.precio,producto.cantidad);
        fread(&producto,sizeof(tproductos),1,productos);
    }
    printf("\n\n");
    fclose(productos);
    continuar();
    system("cls");
    return 0;
}

int idVenta()
{
    int x;
    FILE *factura;
    factura = fopen ("factura.bin", "rb");
    if (factura==NULL) return 0;

    venta V;

    while(!feof(factura))
    {
        fread(&V,sizeof(venta),1,factura);
    }

    if(V.id<0 || V.id>100)
    {
        x = 1;
    }
    else
    {
        x = V.id;
        x++;
    }
    fclose(factura);
    return (x);
}

void registrarVenta()
{
    system("cls");
    char fechaH[11];
    char opc;
    int cant=0;
    int existe =0;
    int cod = 0;
    int x=0;
    float aux=0;
    int contadorProductos=0;

    FILE *factura;
    factura = fopen("factura.bin", "ab");
    if (factura==NULL)
        return 0;

    FILE *productos;
    productos = fopen ("productos.bin", "r+b");
    if (productos==NULL)
    {
        system("cls");
        printf("No hay nada en el inventario para vender :(\n\n\n");
        continuar();
        system("cls");
        return 0;
    }

    tproductos producto;
    venta V;

    x = idVenta();
    V.id = x;
    fecha(fechaH);
    strcpy(V.fecha, fechaH);
    fflush(stdin);
    printf("Ingresa tu nombre: ");
    gets(&V.nombre);
    V.total = 0;

    do
    {
        listaProductos();

        fseek(productos,0,SEEK_SET);
        printf("\nescoge el codigo del producto:");
        fflush(stdin);
        scanf("%d",&cod);
        fflush(stdin);


        while(!feof(productos))
        {
            if (cod==producto.codigo)
            {
                printf("\n%d %s %.f %d\n\n", producto.codigo, producto.descripcion, producto.precio, producto.cantidad);
                printf("Ingresa la cantidad:");
                scanf("%d",&cant);
                if (cant >= producto.cantidad)
                {
                    printf("\nNo hay suficientes unidades solo hay --> %d <-- unidades disponibles de %s\n\n", producto.cantidad, producto.descripcion);
                    printf("Ingresa la cantidad:");
                    scanf("%d",&cant);
                }
                producto.cantidad = producto.cantidad - cant;

                V.listProduct[contadorProductos].cantidad = cant;
                strcpy(V.listProduct[contadorProductos].descripcion,producto.descripcion);
                V.listProduct[contadorProductos].precio = producto.precio;
                V.listProduct[contadorProductos].codigo = 0;

                V.total += producto.precio * cant;


                fflush(stdin);
                int pos=ftell(productos)-sizeof(tproductos);
                fseek(productos,pos,SEEK_SET);
                fwrite(&producto, sizeof(tproductos), 1, productos);
                printf("-->%d\n",producto.cantidad);

                existe=1;
                break;
            }
            fread(&producto, sizeof(tproductos), 1, productos);
        }

        if (existe == 0)
        {
            printf("\nERROR, El codigo del producto no existe\n\n");
        }
        printf("quieres comprar otro producto? [s/n]");
        contadorProductos++;
        scanf("%c", &opc);
    }
    while(opc == 'S' || opc == 's');
    printf("contador---%d", contadorProductos);
    fwrite(&V,sizeof(venta),1,factura);
    fclose(productos);
    fclose(factura);
    system("pause");
    system("cls");

    return 0;

}

void listaVentas()
{
    system("cls");
    int num=1;
    FILE *factura;
    factura = fopen ("factura.bin", "rb");
    if (factura==NULL)
    {
        system("cls");
        printf("No hay ventas :(\n\n\n");
        continuar();
        system("cls");
        return 0;
    }

    venta V;

    fread(&V,sizeof(V),1,factura);
    while(!feof(factura))
    {
        int x = 0;
        int id;
        if(id != V.id)
        {
            printf("\n ============= #%d ============= \n        fecha: %s\n        nombre: %s\n ============================== \n",V.id,V.fecha,V.nombre);
        }

        id = V.id;

        do
        {
            printf(" cantidad comprada: %d\n nombre del producto:%s\n precio del producto: %.f\n ============================== \n",V.listProduct[x].cantidad,V.listProduct[x].descripcion,V.listProduct[x].precio);
            x++;
        }
        while(strlen(V.listProduct[x].descripcion)!=0);

        printf("      total a pagar: %.f\n ============================== \n\n\n",V.total);
        fread(&V,sizeof(V),1,factura);
    }

    fclose(factura);
    continuar();
    system("cls");
    return 0;
}

void bienvenida(){
    printf("###############################\n##            MINI           ##\n##     CAJA REGISTRADORA     ##\n###############################\n");
}

void inventario()
{
    system("cls");
    int opcion;
    do
    {
        printf("#############################\n##                         ##\n##        INVENTARIO       ##\n##                         ##\n#############################\n");
        printf("#   1 - Crear Producto      #\n");
        printf("#   2 - Lista de Productos  #\n");
        printf("#   3 - Regresar            #\n");
        printf("#############################\n");
        printf("     Ingrese su opcion: ");
        scanf("%d",&opcion);
        switch (opcion)
        {
        case 1:
            crearProducto();
            break;
        case 2:
            listaProductos();
            break;
        }
    }
    while(opcion != 3);
    system("cls");
    return 0;
}

int main()
{
    system("cls");
    int opcion;
    do
    {
        bienvenida();
        printf("#     1 - Registrar Venta     #\n");
        printf("#     2 - Inventario          #\n");
        printf("#     3 - Lista de Ventas     #\n");
        printf("#     4 - Salir               #\n");
        printf("###############################\n");
        printf("      Ingrese su opcion: ");
        scanf("%d", &opcion);
        switch (opcion)
        {
        case 1:
            registrarVenta();
            break;
        case 2:
            inventario();
            break;
        case 3:
            listaVentas();
            break;
        }
    }
    while(opcion != 4);
    return 0;
}

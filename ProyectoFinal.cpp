#include <iostream>
#include <windows.h>
#include <sqlext.h>

using namespace std;

//------------------------------------------------------------
// Función para imprimir cualquier tabla de SQL Server
//------------------------------------------------------------
void imprimirTabla(SQLHDBC hDbc, const wchar_t* titulo, const wchar_t* consulta)
{
    SQLHSTMT hStmt;
    SQLRETURN ret;

    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);

    ret = SQLExecDirect(hStmt, (SQLWCHAR*)consulta, SQL_NTS);

    if (!SQL_SUCCEEDED(ret))
    {
        wcout << L"\nError al consultar la tabla " << titulo << endl;
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }

    wcout << L"\n==========================================================" << endl;
    wcout << titulo << endl;
    wcout << L"==========================================================\n" << endl;

    SQLSMALLINT columnas;

    SQLNumResultCols(hStmt, &columnas);

    //---------------------------------------------------------
    // Imprimir encabezados
    //---------------------------------------------------------
    for (SQLUSMALLINT i = 1; i <= columnas; i++)
    {
        SQLWCHAR nombreColumna[100];
        SQLSMALLINT longitud;

        SQLDescribeCol(
            hStmt,
            i,
            nombreColumna,
            100,
            &longitud,
            NULL,
            NULL,
            NULL,
            NULL);

        wcout << nombreColumna << L"\t";
    }

    wcout << endl;

    //---------------------------------------------------------
    // Imprimir registros
    //---------------------------------------------------------
    while (SQLFetch(hStmt) == SQL_SUCCESS)
    {
        for (SQLUSMALLINT i = 1; i <= columnas; i++)
        {
            SQLWCHAR dato[250];

            SQLGetData(
                hStmt,
                i,
                SQL_C_WCHAR,
                dato,
                sizeof(dato),
                NULL);

            wcout << dato << L"\t";
        }

        wcout << endl;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

int main()
{
    SQLHENV hEnv;
    SQLHDBC hDbc;
    SQLRETURN ret;

    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv);

    SQLSetEnvAttr(
        hEnv,
        SQL_ATTR_ODBC_VERSION,
        (SQLPOINTER)SQL_OV_ODBC3,
        0);

    SQLAllocHandle(
        SQL_HANDLE_DBC,
        hEnv,
        &hDbc);

    ret = SQLDriverConnect(
        hDbc,
        NULL,
        (SQLWCHAR*)L"DRIVER={ODBC Driver 17 for SQL Server};SERVER=localhost;DATABASE=EmpresaUNI;Trusted_Connection=Yes;",
        SQL_NTS,
        NULL,
        0,
        NULL,
        SQL_DRIVER_COMPLETE);

    if (!SQL_SUCCEEDED(ret))
    {
        cout << "Error al conectar con la base de datos." << endl;
        return 0;
    }

    cout << "Conexion realizada correctamente.\n";
    //----------------------------------------------------------
// TABLA EMPLEADO
//----------------------------------------------------------
    imprimirTabla(
        hDbc,
        L"TABLA EMPLEADO",
        L"SELECT * FROM Empleado"
    );

    //----------------------------------------------------------
    // TABLA PUESTO
    //----------------------------------------------------------
    imprimirTabla(
        hDbc,
        L"TABLA PUESTO",
        L"SELECT * FROM Puesto"
    );

    cout << "\nPresiona ENTER para continuar..." << endl;
    cin.get();
    //----------------------------------------------------------
// TABLA CENTRO DE TRABAJO
//----------------------------------------------------------
    imprimirTabla(
        hDbc,
        L"TABLA CENTRO DE TRABAJO",
        L"SELECT * FROM CentroTrabajo"
    );

    //----------------------------------------------------------
    // TABLA DIRECTIVO
    //----------------------------------------------------------
    imprimirTabla(
        hDbc,
        L"TABLA DIRECTIVO",
        L"SELECT * FROM Directivo"
    );

    //----------------------------------------------------------
    // Cerrar conexión
    //----------------------------------------------------------
    SQLDisconnect(hDbc);

    SQLFreeHandle(
        SQL_HANDLE_DBC,
        hDbc);

    SQLFreeHandle(
        SQL_HANDLE_ENV,
        hEnv);

    cout << "\n\nConsulta finalizada correctamente." << endl;

    return 0;
}
/******************************************************************************
 *  Nome:   Bruno Rafael Arico                  8125459
 *          Nicolas Nogueira Lopes da Silva     9277541
 *
 *****************************************************************************/

public class EP3 {

    int total;
    int virtual;
    int s;
    int p;
    Queue<Processo> fila;

    public class Processo {
        double t0;
        double tf;
        String nome;
        int b;
        int pid;
        int[] posacessos;
        double[] tempacessos;

        public Processo (String[] strargs, int pidt) {
            t0 = Double.parseDouble(strargs[0]);
            nome = strargs[1];
            tf = Double.parseDouble(strargs[2]);
            b = Integer.parseInt(strargs[3]);
            pid = pidt;
            int size = (strargs.length - 4)/2;
            posacessos = new int[size];
            tempacessos = new double[size];
            for (int i = 4, j = 0; i < strargs.length; i = i + 2, j++) {
                posacessos[j] = Integer.parseInt(strargs[i]);
                tempacessos[j] = Double.parseDouble(strargs[i]);
            }
        }

    }

    // test client
    public static void main(String[] args) {
        String str;
        int pidt = 1;
        total = StdIn.readInt();
        virtual = StdIn.readInt();
        s = StdIn.readInt();
        p = StdIn.readInt();
        fila = new LinkedList<Processo>();
        while (!StdIn.isEmpty()) {
            str = StdIn.readLine();
            String sargs[] = str.split(" ");
            fila.add(new Processo(sargs, pidt));
            pidt++;
        }
    }
}

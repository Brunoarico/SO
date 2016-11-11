/******************************************************************************
 *  Nome:   Bruno Rafael Arico                  8125459
 *          Nicolas Nogueira Lopes da Silva     9277541
 *
 *****************************************************************************/

public class Gerente {

    int total;
    int virtual;
    int s;
    int p;
    int algespaco;
    int algsubstitui;
    double dt;
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

    public void carregarArquivo (String arquivo) {
        String str;
        int pidt = 1;
        In in = new In(arquivo);
        total = in.readInt();
        virtual = in.readInt();
        s = in.readInt();
        p = in.readInt();
        fila = new LinkedList<Processo>();
        while (!in.isEmpty()) {
            str = in.readLine();
            String sargs[] = str.split(" ");
            fila.add(new Processo(sargs, pidt));
            pidt++;
        }
    }


    // test client
    public static void main(String[] args) {
        Gerente gerente = new Gerente();
        while (true) {
            System.out.println("(ep3): ");
            String comando = StdIn.readString();
            if (comando.equals("carrega")) {
                gerente.carregarArquivo (StdIn.readString());
            } else if (comando.equals("espaco")) {
                gerente.algespaco = StdIn.readInt();
            } else if (comando.equals("substitui")) {
                gerente.algsubstitui = StdIn.readInt();
            } else if (comando.equals("executa")) {
                gerente.dt = StdIn.readDouble();
            } else if (comando.equals("sai")) {
                System.out.println("");
                break;
            } else {
                System.out.println("Comando desconhecido\n")
            }
        }
    }
}

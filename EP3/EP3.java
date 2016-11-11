/******************************************************************************
 *  Nome: Nicolas Nogueira Lopes da Silva
 *  Nº USP: 9277541
 *
 *  Compilacao: javac-algs4 PercPlot.java
 *  Execucao: java-algs4 PercPlot N M
 *
 *  N - numero de linhas da matriz NxNxN
 *
 *  M - numero de vezes a gerar uma matriz NxNxN para estimar a probabilidade
 *  de percolacao
 *
 *  Desenha o grafico da probabilidade de percolacao baseada na observacao
 *  experimental contra a probabilidade de cada casa da matriz NxNxN estar
 *  aberta.
 *
 *  % java-algs4 PercPlot 30 10000
 *
 *  Obs.: Arquivo retirado do Sandbox disponibilizado pelo professor.
 *
 ******************************************************************************/

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

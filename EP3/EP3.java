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

        public Processo (String[] strargs) {

        }

    }

    // test client
    public static void main(String[] args) {
        String str;
        total = StdIn.readInt();
        virtual = StdIn.readInt();
        s = StdIn.readInt();
        p = StdIn.readInt();
        fila = new LinkedList<Processo>();
        while (!StdIn.isEmpty()) {
            str = StdIn.readLine();
            String sargs[] = str.split(" ");
            fila.add(new Processo(sargs));
        }
    }
}

/******************************************************************************
 *  Nome:   Bruno Rafael Arico                  8125459
 *          Nicolas Nogueira Lopes da Silva     9277541
 *
 *****************************************************************************/
import java.util.*;

public class Gerente {

    int total;
    int virtual;
    int s;
    int p;
    int algespaco;
    int algsubstitui;
    double dt;
    Queue<Processo> fila;
    BitSet Mtotal;
    BitSet Mvirtual;

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
            for (int i = 4, j = 0; i < strargs.length-1; i = i + 2, j++) {
                posacessos[j] = Integer.parseInt(strargs[i]);
                tempacessos[j] = Double.parseDouble(strargs[i+1]);
            }
        }
    }	

    public void carregarArquivo (String arquivo) {
        System.out.println("Carregando arquivo: " + arquivo + "\n");
        String str;
        int pidt = 1;
        In in = new In(arquivo);
	
	str = in.readLine();
        String sargs[] = str.split(" ");
        total = Integer.parseInt(sargs[0]);
        virtual = Integer.parseInt(sargs[1]);
	
	Mtotal = new BitSet (total);
	Mvirtual = new BitSet (virtual);

	s = Integer.parseInt(sargs[2]);
	p = Integer.parseInt(sargs[3]);

        fila = new LinkedList<Processo>();

        while (!in.isEmpty()) {
            str = in.readLine();
            sargs = str.split(" ");
            fila.add(new Processo(sargs, pidt));
            pidt++;
        }
    }

    public void firstFit (Processo Proc) {
	int size = Proc.b;
	int count = 0, beg = 0;
	boolean flag = false;
	for (int i = 0; i < total; i++){
	    //se o bit for zero
	    
	    if (!Mtotal.get(i) && count < size) {
		//marca o comeco
		if (!flag) {beg = i; flag = true;}
		count++; //conta a sequencia de zeros
	    }
	    else {
		flag = false;
		//quando a sequencia acaba
		if (count == size) {
		    //se for maior que size, aloca
		    for (int j = 0; j < size; j++){
			Mtotal.set(beg+j);
		    }
		    break; // encerra a busca
		}
		else count = 0;
	    }
	}
	
	System.out.println(Mtotal.toString());
    }
    
    // test client
    public static void main(String[] args) {
        Gerente gerente = new Gerente();
        while (true) {
            System.out.print("(ep3): ");
            String comando = StdIn.readString();
            if (comando.equals("carrega")) {
                gerente.carregarArquivo (StdIn.readString());
		gerente.firstFit(gerente.fila.remove());
		System.out.println("tails");
		gerente.firstFit(gerente.fila.remove());
            } else if (comando.equals("espaco")) {
                gerente.algespaco = StdIn.readInt();
                System.out.println(gerente.algespaco);
            } else if (comando.equals("substitui")) {
                gerente.algsubstitui = StdIn.readInt();
                System.out.println(gerente.algsubstitui);
            } else if (comando.equals("executa")) {
                gerente.dt = StdIn.readDouble();
                System.out.println(gerente.dt);
            } else if (comando.equals("sai")) {
                System.out.println("saiu!");
                break;
            } else {
                System.out.println("Comando desconhecido\n");
            }
        }
    }
}

/******************************************************************************
 *  Nome:   Bruno Rafael Arico                  8125459
 *          Nicolas Nogueira Lopes da Silva     9277541
 *
 *****************************************************************************/
import java.util.*;
import java.lang.*;

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
    static int[] bind = new int[virtual];
    

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

    public int firstFit (Processo Proc) {
        int size = (int)Math.ceil((double)Proc.b/(double)s); //numero de blocos que ira ocupar
	int count = 1, beg = 0;
	
	for (int i = 0; i < virtual; i++) {
	    if (!Mvirtual.get(i)) {
		beg = i;
		while (!Mvirtual.get(i) && i < virtual) {i++; count++;}
		if(count > size) {
		    Mvirtual.set(beg, beg+size);
		    System.out.println(Mvirtual.toString() + " " + Mvirtual.length());
		    return beg;
		}
		else count = 0;
	    }
	}
	System.out.println("Não foi possivel alocar a memoria");
	System.exit(-1);
	return beg;
    }

    static int here = 0;
    
    public int nextFit (Processo Proc) {
        int size = (int)Math.ceil((double)Proc.b/(double)s); //numero de blocos que ira ocupar
	int count = 1, beg = 0, i, ii;
	boolean flag = false;
	for (i = here; i < here + virtual ; i++){
	    ii = (i % virtual); //para fazer nosso apontador ver toda a memoria
	    if (ii == 0) count = 1; //a memoria não é circular
	    if (!Mvirtual.get(ii) && count < size) { //se o bit for zero
		if (!flag) {beg = ii; flag = true;} //marca o comeco
		count++; //conta a sequencia de zeros
	    }
	    else { //quando a sequencia acaba
		flag = false;
		if (count >= size) { //se for maior que size, aloca
		    Mvirtual.set(beg, beg+size);
		    here = beg+size; //marca onde parou de olhar
		    System.out.println(Mvirtual.toString() + " " + Mvirtual.length());
		    return beg; // encerra a busca
		}
		else count = 0; //senão zera o contador e continua buscando
	    }
	}
        System.out.println("Não foi possivel alocar a memoria");
	System.exit(-1);
	return beg;
    }

    public int bestFit (Processo Proc) {
	int size = (int)Math.ceil((double)Proc.b/(double)s); //numero de blocos que ira ocupar
	int provbeg = 0, realbeg = -1, count = 0, diff = Integer.MAX_VALUE;

	for (int i = 0; i < virtual; i++) {
	    if (!Mvirtual.get(i)) {
		provbeg = i;
		while (!Mvirtual.get(i) && i < virtual) {i++; count++;}

		if (count > size && count-size < diff) {
		    realbeg = provbeg;
		    diff = count-size;
		}
		else count = 0;
	    }
	}

	if (realbeg < 0) {
	    System.out.println("Não foi possivel alocar a memoria");
	    System.exit(-1);
	}
	else {
	    Mvirtual.set(realbeg, realbeg+size);
	    System.out.println(Mvirtual.toString() + " " + Mvirtual.length());
	    return realbeg;
	}
	return realbeg;
    }

    public int worstFit (Processo Proc) {
	int size = (int)Math.ceil((double)Proc.b/(double)s); //numero de blocos que ira ocupar
	int provbeg = 0, realbeg = -1, count = 0, diff = -1;

	for (int i = 0; i < virtual; i++) {
	    if (!Mvirtual.get(i)) {
		provbeg = i;
		while (!Mvirtual.get(i) && i < virtual) {i++; count++;}

		if (count > size && count-size > diff) {
		    realbeg = provbeg;
		    diff = count-size;
		}
		else count = 0;
	    }
	}

	if (realbeg < 0) {
	    System.out.println("Não foi possivel alocar a memoria");
	    System.exit(-1);
	}
	else {
	    Mvirtual.set(realbeg, realbeg+size);
	    System.out.println(Mvirtual.toString() + " " + Mvirtual.length());
	    return realbeg;
	}
	return realbeg;
    }

    public void binding (int VirtPage, int RealPage) {
        try:
	    bind[VirtPage] = RealPage;
	catch:
	    System.out.println("Binding errado");
    }

    public getFromVirtual (int adr) {

    


    // test client
    public static void main(String[] args) {
        Gerente gerente = new Gerente();
        while (true) {
            System.out.print("(ep3): ");
            String comando = StdIn.readString();
            if (comando.equals("carrega")) {
                gerente.carregarArquivo (StdIn.readString());
		System.out.println("Sonic");
		System.out.println("A posição é:" + gerente.worstFit(gerente.fila.remove()));
		System.out.println("tails");
		System.out.println("A posição é:" + gerente.worstFit(gerente.fila.remove()));
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

/******************************************************************************
 *  Nome:   Bruno Rafael Arico                  8125459
 *          Nicolas Nogueira Lopes da Silva     9277541
 *
 *****************************************************************************/
import java.util.*;
import java.lang.*;
import java.io.*;

public class Gerente {

    static int total; //memoria real
    static int virtual; //memoria virtual
    int s; //tamanho da unidade de alocação
    int p; //tamanho da pagina
    int algespaco; 
    int algsubstitui;
    double dt;
    Queue<Processo> fila;
    BitSet Mtotal; //bitmap da memoria real
    BitSet Mvirtual; //bitmap da memoria virtual
    int realpages; //numero de paginas que cabem na Real
    int virtualpages; //numero de paginas que cabem na Virtual
    static ArrayList<Queue<Double>> countoptimal;
    TreeMap<Integer, Integer> bindr;
    TreeMap<Integer, Integer> bindv;
    boolean pagebit[];
    ArrayList<Cell> eventos;
    static Queue<Integer> SC = new LinkedList<Integer>();
	
    public class Processo {
        double t0;
        double tf;
        String nome;
        int b;
        int pid;
	int offset;
        int[] posacessos;
        double[] tempacessos;

	public void setOffset(int value) {
	    offset = value;
	}
	
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

        virtualpages =(int) (((double)virtual/(double)s)/(double)p);
	realpages = (int)(((double)total/(double)s)/(double)p);
	pagebit = new boolean[virtualpages];
	countoptimal = new ArrayList<Queue<Double>>();

	for(int i = 0; i < virtualpages; i++) 
	    countoptimal.add(i, new LinkedList<Double>()); 

	bindv = new TreeMap<Integer, Integer>();
        bindr = new TreeMap<Integer, Integer>();
	fila = new LinkedList<Processo>();
	
        while (!in.isEmpty()) {
            str = in.readLine();
            sargs = str.split(" ");
	    fila.add(new Processo(sargs, pidt));
            pidt++;
        }
	
	fprintInicial("/tmp/ep3.mem", total);
        fprintInicial("/tmp/ep3.vir", virtual);
    }

    public void firstFit (Processo Proc) {
	int tam = Proc.b;                                                      
	int size = 0;                                                        
	int count = 0, beg = 0;
	while (size <= tam) size += p;                             //somo p até fechar a quantidade minima de paginas
	Proc.b = size;
	for (int i = 0; i < virtual; i++) {                         
	    if (!Mvirtual.get(i)) {                      
		beg = i;
		while (!Mvirtual.get(i) && i < virtual) {i++; count++;}
		if(count > size) {
		    Mvirtual.set(beg, beg+size);
		    //imprimindo no arquivo da memoria virtual
		    for (int j = beg; j < beg+size; j++) {
		    	fprintPagina("/tmp/ep3.vir", Proc.pid, j, s);
		    }
		    Proc.setOffset(beg);
		    return;
		}
		else count = 0;
	    }
	}
	System.out.println("Não foi possivel alocar a memoria de tam " + size);
	System.exit(-1);
        return;
    }

    static int here = 0;
    
    public void nextFit (Processo Proc) {
	int tam = Proc.b;
	int size = 0;
	int count = 0, beg = 0, i, ii;
	boolean flag = false;
	while (size <= tam) size += p;
	Proc.b = size;
	for (i = here; i < here + virtual ; i++){
	    ii = (i % (virtual-1));                              //para fazer nosso apontador ver toda a memoria
	    if (ii == 0) count = 0;                              //a memoria não é circular
	    if (!Mvirtual.get(ii) && count < size) {             //se o bit for zero
		if (!flag) {beg = ii; flag = true;}              //marca o comeco
		count++;                                         //conta a sequencia de zeros
	    }
	    else {                                               //quando a sequencia acaba
		flag = false;
		if (count >= size) {                             //se for maior que size, aloca
		    Mvirtual.set(beg, beg+size);
		    //imprimindo no arquivo da memoria virtual
		    for (int j = beg; j < beg+size; j++) {
		    	fprintPagina("/tmp/ep3.vir", Proc.pid, j, s);
		    }
		    here = beg+size+1; //marca onde parou de olhar
		    Proc.setOffset(beg); // encerra a busca
		    return;
		}
		else count = 0; //senão zera o contador e continua buscando
	    }
	}
        System.out.println("Não foi possivel alocar a memoria para " + size);
	System.exit(-1);
	return;
    }

    public void bestFit (Processo Proc) {
	int tam = Proc.b;
	int size = 0;
	int provbeg = 0, realbeg = -1, count = 0, diff = Integer.MAX_VALUE;
	while (size <= tam) size += p;
	Proc.b = size;
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
		//imprimindo no arquivo da memoria virtual
		for (int j = realbeg; j < realbeg+size; j++) {
		    fprintPagina("/tmp/ep3.vir", Proc.pid, j, s);
		}
	    Proc.setOffset(realbeg);
	}
	return;
    }

    public void worstFit (Processo Proc) {
	int tam = Proc.b;
	int size = 0;
	int provbeg = 0, realbeg = -1, count = 0, diff = -1;
	while (size <= tam) size += p;
	Proc.b = size;
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
		//imprimindo no arquivo da memoria virtual
		for (int j = realbeg; j < realbeg+size; j++) {
		    fprintPagina("/tmp/ep3.vir", Proc.pid, j, s);
		}
		Proc.setOffset(realbeg);
	}
	return;
    }

    //mapeia uma pagina da Virtual na memoria Real
    public void binding (int Virtual, int Real) {
	if(bindr.containsKey(Real)) bindv.remove(bindr.get(Real)); // REMOVE V -> R
        bindv.put(Virtual, Real); // ADICIONA NV -> R
        bindr.put(Real, Virtual); // MODIFICA R -> V para R -> NV

    }

    //mata um processo
    public void kill (Processo Proc) { 
	int vblocks, rblocks;
	Mvirtual.set(Proc.offset, Proc.offset+Proc.b, false); 
	for (int j = Proc.offset; j < Proc.offset+Proc.b; j++) {
	    fprintPagina("/tmp/ep3.vir", -1, j, s);
	}	
	vblocks = (int)Proc.offset/s/p;                               
	while (vblocks <= (int)(Proc.offset+Proc.b)/s/p) {
	    if (bindv.containsKey(vblocks)) {
		rblocks = bindv.get(vblocks);               
		Mtotal.set(rblocks*p, rblocks*p+p, false);      
		for (int j = rblocks*p; j < rblocks*p+p; j++) {
		    fprintPagina("/tmp/ep3.mem", -1, j, s);
		}		
		bindv.remove(vblocks);                     
		bindr.remove(rblocks);                     
	    }
	    vblocks += 1;
	}
    }


    //recebe uma posição na memoria virtual pos
    public void optimal (int pos, int pid) {
        int accblk = (int) (pos/s/p)%countoptimal.size(), index = 0;
	double buffer, larger = 0;

	
	if(!bindv.containsKey(accblk)) {

	    if(bindr.size() < realpages){                 //ve se tem espaço livre na memoria real
	        for(int i = 0; i < realpages; i++)        //se tiver procura
		    if(!bindr.containsKey(i)){
			Mtotal.set(i*p, (i+1)*p); //preenche esse bloco do bitset
			//imprimindo no arquivo da memoria virtual
			for (int j = i*p; j < (i+1)*p; j++) {
			    fprintPagina("/tmp/ep3.mem", pid, j, s);
			}
			binding(accblk, i);
			return;
		    }
	    }
	    else {
		for(int i : bindr.values()) { //percorre todos que estão na real
		    if(countoptimal.get(i).isEmpty()) { //se a pilha de alguem estiver vazia
			binding(accblk, bindv.get(i)); //associo o novo bloco ao da antiga
			return;
		    }
		    else { //senão procuro pelo que vai ficar mais tempo sem acesso
			buffer = countoptimal.get(i).peek(); //olha o topo da fila
			if (buffer >= larger) {larger = buffer; index = i;} //se o acesso for mais demorado troc
		    }   
		}
		binding (accblk, bindv.get(index));
		return;
	    }	    
	}
    }
    
    
    public void secondChance (int pos, int pid) {
	int accblk = (int)pos/s/p;
	int buffer, firstbuffer;
	System.out.println("pos " + pos + " accblk " + accblk);
	if(!bindv.containsKey(accblk)) {
	     if(bindr.size() < realpages){
		 for (int i = 0; i < realpages; i++) //se tiver procura
		     if(!bindr.containsKey(i)){ //encontrou 
			 Mtotal.set(i*p, (i+1)*p); //preenche
			//imprimindo no arquivo da memoria virtual
			for (int j = i*p; j < (i+1)*p; j++) {
			    fprintPagina("/tmp/ep3.mem", pid, j, s);
			    System.out.println("escrevendo");
			}
			System.out.println("Associa " + accblk + " a " + i);
			binding (accblk, i); //faz a associação
			SC.add(accblk); //entra na fila
			System.out.println("Tinha espaço");
			System.out.println(Mtotal.toString());
			pagebit[accblk] = true; //liga o bit de acesso
			return;
		     }
	     }
	     
	     else {
		 buffer = firstbuffer = SC.remove(); //tira da fila
		 while (pagebit[buffer]){ //olha o bit
		     System.out.println("O bit de " + buffer + "é true");
		     SC.add(buffer); //se for true devolve 
		     buffer = SC.remove(); //tenta o proximo
		     if(buffer == firstbuffer) break; //se voltou no primeiro usa o ele
		 
		 }
		 System.out.println("O espaço foi alocado "+ buffer);
		 System.out.println(Mtotal.toString());
		 binding (accblk, bindv.get(buffer));
		 return;
	     }	 
	}
	else{
	    pagebit[accblk] = true; //so liga o bit de acesso
	    System.out.println("acessou a pagina " + accblk );
	    return;
	}
    }

    static LinkedList<Integer> CList = new LinkedList<Integer>();
    static int pointer = 0;
    
    public void clock(int pos, int pid) {
	int accblk = pos/s/p, buffer;
	if(!bindv.containsKey(accblk)) {
	     if(bindr.size() < realpages){
		 for (int i = 0; i < realpages; i++) //se tiver procura
		     if(!bindr.containsKey(i)){ //encontrou 
			 Mtotal.set(i*p, (i+1)*p); //preenche
			//imprimindo no arquivo da memoria virtual
			for (int j = i*p; j < (i+1)*p; j++) {
			    fprintPagina("/tmp/ep3.mem", pid, j, p);
			}
			 binding (accblk, i); //faz a associação
			 CList.add(accblk); //entra na lista
			 pagebit[accblk] = true; //liga o bit de acesso
		     }
	     }
	     
	     else {
		 buffer = pointer;
		 while (pagebit[CList.get(pointer)]) { //olha o bit
		     pointer = (pointer + 1) % CList.size(); //é circular
		     if (buffer == pointer) break; //não entra em loop infinito
		 }
		 binding (accblk, bindv.get(CList.remove(pointer))); //associa
	     }	 
	}
	else pagebit[accblk] = true;
    }
    //////////////////////// LEITURA ////////////////////////////////

    public int readPid (String nome, int pagina, int tamanhopag) {
    	int id = -1;
        try {
            RandomAccessFile file = new RandomAccessFile(nome, "rw");
            file.seek(tamanhopag*pagina);
            id = file.readInt();
            file.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return id;    	
    }

    //////////////////////// PRINTS /////////////////////////////////

    //inicializa uma das memórias com -1
    public void fprintInicial (String nome, int tamanho) {
        try {
            RandomAccessFile file = new RandomAccessFile(nome, "rw");
            int numero = -1;
            for (int i = 0; i < tamanho; i++)
                file.writeInt(numero);
            file.close();
            System.out.println("Arquivo " + nome + " inicializado com " + numero);
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void fprintPagina (String nome, int pid, int pagina, int tamanhopag) {
        try {
            RandomAccessFile file = new RandomAccessFile(nome, "rw");
            file.seek(tamanhopag * pagina * 4);
            for (int i = 0; i < tamanhopag; i++){
                file.writeInt(pid);
	    }
            file.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    //printa o arquivo de caminho nome
    public void printArquivo (String nome) {
        int bytef;
        try {
            RandomAccessFile file = new RandomAccessFile(nome, "r");
	    System.out.println(file.length());
            for (int i = 0; i < file.length()/4; i++) {
                bytef = file.readInt();
		System.out.print(bytef + " ");
                //System.out.print(String.format("%8s", Integer.toBinaryString(bytef)).replace(' ', '0'));
                //System.out.print(" ");
            }

        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void imprimir () {
        System.out.println("\n Arquivo Memória Virtual");
        printArquivo("/tmp/ep3.vir");
        System.out.println("\nBitmap da memória virtual");
        printBitSet(Mvirtual);
        //System.out.println("\nArquivo Memória física");
        //printArquivo("/tmp/ep3.mem");
        //System.out.println("\nBitmap da memória física");
        //printBitSet(Mtotal);
        System.out.println("---");
    }

    //printa o estado da memória virtual
    public void printBitSet(BitSet memoria) {
        for (int i = 0; i < memoria.size(); i++) {
            if (memoria.get(i))
                for (int j = 0; j < s*p; j++)
                    System.out.print("1");
            else
                for (int j = 0; j < s*p; j++)
                    System.out.print("0");
        }
        System.out.print("\n");
    }

    //////////////////////////////// executar /////////////////////////////////

    //clase interna criada para ordenar eventos em função do tempo

    public class Cell implements Comparable<Cell> {
        public final Processo proc;
        public final double tempo;
        public final int pos;	//se evento for 0 ou 2 vale 0
        public final int tipoevento; //0 - inicializa, 1 - acesso, 2 - destroi

        public Cell(Processo proc, double tempo, int pos, int tipoevento) {
            this.proc = proc;
            this.tempo = tempo;
            if (tipoevento == 0 || tipoevento == 2)
            	this.pos = 0;
            else
            	this.pos = pos;
            this.tipoevento = tipoevento;
        }

       
        public int compareTo(Cell other) {
            return Double.valueOf(this.tempo).compareTo(other.tempo);
        }
    }

    public void executar () {
	double dtR = 2;
    	eventos = new ArrayList<Cell>();
    	for (Processo proc : this.fila) {
	    eventos.add(new Cell (proc, proc.t0, 0, 0));
    		for (int i = 0; i < proc.posacessos.length; i++)
		    eventos.add(new Cell (proc, proc.tempacessos[i], proc.posacessos[i], 1));
    		eventos.add(new Cell (proc, proc.tf, 0, 2));
    	}
	
    	Collections.sort(eventos); 
    	double tpassado = 0, tzerarbitR = 0 ;
    	imprimir();
    	for (Cell celula : eventos) {
    		while (tpassado < celula.tempo && tzerarbitR <= celula.tempo) {
		    if (tpassado <= celula.tempo) {
			imprimir();
			tpassado += dt;
		    }
    		
		    if (tzerarbitR <= celula.tempo) {
			for(int i =0; i < pagebit.length; i++) pagebit[i] = false;
			tzerarbitR += dtR;
		    }
		}  
		
    		switch (celula.tipoevento) {
    			case 0:
    				//criar processo na memoria
    				switch (algespaco) {
    					case 1:
    						firstFit(celula.proc);
    						break;
    					case 2:

    						nextFit(celula.proc);
    						break;
    					case 3:

    						bestFit(celula.proc);
    						break;
    					case 4:

    						worstFit(celula.proc);
    						break;
    					default:
    						System.out.println("ALGO DEU ERRADO no switch algespaco");
    						break;
    				} 
    				break;
    			case 1:
    				//acessar endereço na memoria
    				switch (algsubstitui) {
    					case 1:

    						optimal(celula.pos + celula.proc.offset, celula.proc.pid);
    						break;
    					case 2:
					        secondChance(celula.pos + celula.proc.offset, celula.proc.pid);
    						break;
    					case 3:

					        clock(celula.pos + celula.proc.offset, celula.proc.pid);
    						break;
    					case 4:
					        System.out.println("Não implementado =/");
    						break;
    					default:
    						System.out.println("ALGO DEU ERRADO no switch algsubstitui");
    						break;
    				}
    				break;
    			case 2:
			   
			    kill(celula.proc);


    				break;
    			default:
    				System.out.println("ALGO DEU ERRADO no switch celula.tipoevento");

    				break;
    		}


    	}
	System.out.println("Estado final");
	imprimir();
    }
    
	
    // test client
    public static void main(String[] args) {
        Gerente gerente = new Gerente();
        while (true) {
            System.out.print("(ep3): ");
            String comando = StdIn.readString();
            if (comando.equals("carrega")) {
                gerente.carregarArquivo (StdIn.readString());
            } else if (comando.equals("espaco")) {
                gerente.algespaco = StdIn.readInt();
                System.out.println(gerente.algespaco);
            } else if (comando.equals("substitui")) {
                gerente.algsubstitui = StdIn.readInt();
                System.out.println(gerente.algsubstitui);
            } else if (comando.equals("executa")) {
                gerente.dt = StdIn.readDouble();
		gerente.executar();
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

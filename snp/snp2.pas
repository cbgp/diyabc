program snp1;

{$APPTYPE CONSOLE}
{$R+}
uses
  SysUtils,
  Classes,
  UnitMesUtils_5 in 'UnitMesUtils_5.pas';
 
const
  ah:array[0..2]of extended =(0.2162,0.6102,0.3046);
  bh:array[0..2]of extended =(0.4690,0.3842,0.3753);
  af:array[0..2]of extended =(0.5246,0.3389,0.4672);
  bf:array[0..2]of extended =(1.6196,1.2259,2.4070);
  ml:array[0..4]of integer = (1000,5000,10000,20000,40000);
  nbr=10;
  anbr=1.0/nbr;
  nrep=1000;

type
  arrayS =array of string;

var
  gen  :array of array of array of integer;
  geno :array of array of smallint;
  samplesize : array of array of integer;
  indivsexe,indivpop :array of integer;
  indivname :array of string;  
  freq :array of array of extended;
  nloc,npop,rep  :integer;
  nind       :array of integer;
  nindtot,freqmax,nloca,io,iter    :integer;
  popname    :arrayS;
  t0         :TDateTime;
  myHour, myMin, mySec, myMilli : Word;
  pobth,pobtf,pobtn       :array[0..2,0..pred(nbr)] of extended;
  quanth,quantf,quantn    :array[0..2,0..pred(nbr)] of extended;
  ht,nt,ft                :array[0..2] of vecteurF;
  DD         :array[0..2,0..4,0..2,0..pred(nrep)] of extended;
  f2,f3  :textfile;
  randinitOK :boolean;
  GM :TMwcGen;
  wl :vecteurF;
  index  :vecteurI;

procedure splitwords(s:string;sep:string;var nss:integer;var ss:arrayS;lim:integer);
  var
    i,j,j0   :integer;
    s0,s1     :string;
  begin
	j:=0;
	while pos(sep,s)=1 do s:=copy(s,2,length(s));
	nss:=0;
	s:=s+sep;
	s1:='';
	for i:=1 to length(s) do
	  begin
	    s0:=copy(s,i,1);
	    if s0=sep then	
	      begin
	        inc(j);
	        if j=1 then
	          begin
	            s1:=s1+s0;
	            if j=1 then inc(nss);
	            if (lim>0)and(nss=lim) then break;
	          end;
	      end
		  else
		  begin
			s1:=s1+s0;
			j:=0;
		  end;
	  end;
	setlength(ss,nss);
	for i:=0 to pred(nss) do
	  begin
		j0:=pos(sep,s1);
		ss[i]:=copy(s1,1,pred(j0));
		s1:=copy(s1,succ(j0),length(s1));
	  end;
  end;

procedure readfile;
  var
	f0  :textfile;
	i,j,k,nss,io,nli,nl  :integer;
	ligne  :string;
	ss     :arrayS;
	trouve  :boolean;
  begin
	assignfile(f0,'geno_A.txt');reset(f0);
	readln(f0,ligne);
	nloc:=0;
	ligne:=copy(ligne,pos('POP',ligne)+3,length(ligne));
	nloc:=length(ligne) div 2;
    writeln('nloc=',nloc);
    splitwords(ligne,' ',nss,ss,-1);
    writeln('nss=',nss);
    npop:=1;setlength(popname,1);setlength(nind,1);
    readln(f0,ligne);
    splitwords(ligne,' ',nss,ss,3);
	popname[pred(npop)]:=ss[2];
	nind[pred(npop)]:=1;nli:=0;
	while not eof(f0) do
	  begin
		readln(f0,ligne);inc(nli);
		splitwords(ligne,' ',nss,ss,3);
		j:=0;trouve:=false;
	    while (j<npop)and(not trouve) do
	      begin
	        trouve:= ss[2]=popname[j];
	        if not trouve then inc(j);
	      end;
	    if trouve then inc(nind[j]) else
	      begin
			inc(npop);setlength(popname,npop);setlength(nind,npop);
			nind[pred(npop)]:=1;
			popname[pred(npop)]:=ss[2];
	      end;
	      
	  end;
    writeln('npop = ',npop);
    for i:=0 to pred(npop) do write(popname[i],'  (',nind[i],')   ');writeln;
	setlength(gen,npop);for i:=0 to pred(npop) do setlength(gen[i],nind[i],nloc);
	nindtot:=succ(nli);
	writeln('nindtot=',nindtot);
	setlength(indivname,nindtot);
	setlength(indivsexe,nindtot);
	setlength(indivpop,nindtot);
	setlength(geno,nindtot,nloc);
	reset(f0);
	readln(f0,ligne);nl:=0;
	for i:=0 to pred(npop) do
	  begin
		for j:=0 to pred(nind[i]) do
		  begin
			readln(f0,ligne);inc(nl);//writeln('nl=',nl,'   ',copy(ligne,1,20));
			splitwords(ligne,' ',nss,ss,-1);
			//writeln('lecture de ',ss[0],' ',ss[2]);
			if nss<>nloc+3 then begin writeln('ligne incorrecte');halt;end;
			indivname[pred(nl)]:=ss[0];
			indivsexe[pred(nl)]:=0;
			if ss[1]='M' then indivsexe[pred(nl)]:=1;
			if ss[1]='F' then indivsexe[pred(nl)]:=2;
			k:=0;while (ss[2]<>popname[k]) do inc(k);
			indivpop[pred(nl)]:=k;
		    for k:=0 to pred(nloc) do begin val(ss[k+3],gen[i][j][k],io);geno[pred(nl),k]:=gen[i][j][k];end;
		    write(chr(13),'lecture du fichier ',100.0*nl/nli:4:0,'%');
		  end;
	  end;
	writeln;
  end;

{procedure ecribin;
  var
	i,j,k,pop,ind,long :integer;
	fi :TFilestream;
	ss :AnsiString;
  begin
    fi:=TFilestream.Create('geno_A.bin',fmCreate);
    fi.WriteBuffer(nloc,4);
    ss:='A';
    for i:=0 to pred(nloc) do fi.WriteBuffer(Pchar(ss)^,1);
    fi.WriteBuffer(npop,4);
    for i:=0 to pred(npop) do 
	  begin 
		ss:=popname[i];
		long:=length(ss);
		fi.WriteBuffer(long,4);
		fi.WriteBuffer(Pchar(ss)^,long);
	  end;
    for i:=0 to pred(npop) do fi.WriteBuffer(nind[i],4);
    for pop:=0 to pred(npop) do
      begin
		for ind:=0 to pred(nindtot) do
		  begin
			if (indivpop[ind]=pop) then 
			  begin
				fi.WriteBuffer(pop,4);
				fi.WriteBuffer(indivsexe[ind],4);
				ss:=indivname[ind];
				long:=length(ss);
				fi.WriteBuffer(long,4);
				fi.WriteBuffer(Pchar(ss)^,long);
				k:=ind;j:=0;
				for i:=0 to pred(nloc) do fi.WriteBuffer(geno[ind][i],2);
			  end;
		  end;
	  end;
	fi.Destroy;
  end;

procedure libin;
  var
	i,j,k,pop,ind,long :integer;
	fi :TfileStream;
	ss :AnsiString;
	c  :array of ansichar;
  begin
	fi:=TFileStream.Create('geno_A.bin',fmOpenRead);
	fi.ReadBuffer(nloc,4);writeln('nloc=',nloc);
	setlength(c,nloc);writeln('apres setlength');
	fi.Read(c,2);writeln('apres fi.read');
	write(ord(c[0]),' ',c[1]);writeln;
	halt;
	fi.ReadBuffer(npop,4);writeln('npop=',npop);
  end;}

procedure combrank(x,y:vecteurF;var rangx,rangy:vecteurF);
  var
    i,j,n,m  :integer;
  begin
	n:=length(x);m:=length(y);
	setlength(rangx,n);setlength(rangy,m);
	writeln('calcul des rangx (',n,')');
	for i:=0 to pred(n) do
	  begin
	    rangx[i]:=1.0;
	    for j:=0 to pred(n) do
		  begin
		    if j<>i then 
			  begin
			    if x[i]>x[j] then rangx[i]:=rangx[i]+1.0;
			    if x[i]=x[j] then rangx[i]:=rangx[i]+0.5;
		      end;
		  end;
	    for j:=0 to pred(m) do
		  begin
		    if x[i]>y[j] then rangx[i]:=rangx[i]+1.0;
		    if x[i]=y[j] then rangx[i]:=rangx[i]+0.5;
		  end;
	  end;
	  writeln('calcul des rangy (',m,')');
 	for i:=0 to pred(m) do
	  begin
	    rangy[i]:=1.0;
	    for j:=0 to pred(m) do
		  begin
		    if j<>i then 
			  begin
			    if y[i]>y[j] then rangy[i]:=rangy[i]+1.0;
			    if y[i]=y[j] then rangy[i]:=rangy[i]+0.5;
		      end;
		  end;
	    for j:=0 to pred(n) do
		  begin
		    if y[i]>x[j] then rangy[i]:=rangy[i]+1.0;
		    if y[i]=x[j] then rangy[i]:=rangy[i]+0.5;
		  end;
	  end;
 end;

procedure combrank2(x,y:vecteurF;var rangx,rangy:vecteurF);
  var
    A  :matriceF;
    i,m,n,k :integer;
  begin
	n:=length(x);m:=length(y);
	setlength(rangx,n);setlength(rangy,m);
	setlength(A,n+m,3);
	for i:=0 to pred(n) do begin A[i,0]:=x[i];A[i,1]:=0;A[i,2]:=i;end;
	for i:=0 to pred(m) do begin A[n+i,0]:=y[i];A[n+i,1]:=1;A[n+i,2]:=i;end;
    fsortV('a',n+m,3,0,A);
    for i:=0 to pred(n+m) do
      begin
		k:=round(A[i,2]);
        if A[i,1]<0.5 then rangx[k]:=succ(i)
                      else rangy[k]:=succ(i);
	  end;
	{writeln('calcul des rangx (',n,')');
    for i:=0 to pred(n) do
      begin
		k:=0;while not((A[k,1]=0)and(A[k,2]=i)) do inc(k);
		rangx[i]:=succ(k);
      end;
	  writeln('calcul des rangy (',m,')');
    for i:=0 to pred(m) do
      begin
		k:=0;while not((A[k,1]=1)and(A[k,2]=i)) do inc(k);
		rangy[i]:=succ(k);
      end;}
  end;

procedure calfreq;
  var
    i,j,k,n  :integer;
    s :extended;
    
  begin
    setlength(freq,npop);for i:=0 to pred(npop) do setlength(freq[i],nloc);
    setlength(samplesize,npop);for i:=0 to pred(npop) do setlength(samplesize[i],nloc);
    
    for i:=0 to pred(npop) do
      begin
        for k:=0 to pred(nloc) do
          begin
            n:=0;s:=0.0;
            for j:=0 to pred(nind[i]) do if gen[i][j][k]<>9 then 
			  begin 
				n:=n+2;
				s:=s+gen[i][j][k];
			  end;
            if n>0 then freq[i][k]:=s/n else freq[i][k]:=0.0;
            samplesize[i][k]:=n;
          end;
          
      end;
  end;

procedure calhet(tout:boolean);
  var
    i,k,nl,j,loc,q,nlocu,N,M  :integer;
    hetmoy,hetvar,sx,sx2,a,b,he,rangx,rangy  :vecteurF;
    f1  :textfile;
    locOK :array of array of boolean;
    het  :array of array of extended;
	pob,quant :array of extended;
	sw,Dkl,x,Dabs,Deuc,Dqua,Dcra,U,V  :extended;
  begin
    if tout then nlocu:=nloc else nlocu:=nloca;
    //writeln('nloc=',nloc,'   nlocu=',nlocu);
    setlength(het,npop);for i:=0 to pred(npop) do setlength(het[i],nloc);setlength(he,nlocu);
    setlength(hetmoy,npop);setlength(hetvar,npop);setlength(sx,npop);setlength(sx2,npop);setlength(a,npop);setlength(b,npop);
    setlength(locOK,npop,nloc);
    setlength(pob,nbr);setlength(quant,nbr);
    for k:=0 to pred(npop) do for loc:=0 to pred(nlocu) do locOK[k][index[loc]]:=true;
    for k:=0 to pred(npop) do
      begin
        sx[k]:=0.0;sx2[k]:=0.0;nl:=0;for i:=0 to pred(nbr) do pob[i]:=0.0;
        for loc:=0 to pred(nlocu) do
          begin
            het[k][index[loc]]:=0.0;
            if samplesize[k][index[loc]]>40 then
              begin
                inc(nl);locOK[k][index[loc]]:=true;
				het[k][index[loc]]:=2.0*(1.0 - sqr(freq[k][index[loc]])-sqr(1.0-freq[k][index[loc]])){*samplesize[k][index[loc]]/pred(samplesize[k][index[loc]])};
				sx[k]:=sx[k]+het[k][index[loc]];
				sx2[k]:=sx2[k]+sqr(het[k][index[loc]]);
				i:=trunc(nbr*het[k][index[loc]]);
				if (i>pred(nbr)) then i:=pred(nbr);
				pob[i]:=pob[i]+1.0;
			  end else locOK[k][index[loc]]:=false;
          end;
        for loc:=0 to pred(nlocu) do he[loc]:=het[k][index[loc]];
        if tout then begin setlength(ht[k],nlocu);for loc:=0 to pred(nlocu) do ht[k][loc]:=het[k][index[loc]];end;
        fsort('a',nlocu,he);
        for i:=0 to pred(nbr) do begin q:= succ(i)*nlocu div succ(nbr);quant[i]:=he[q];end;
        {if tout then write('TOUT ');
        write('Het (',k,')   ');
        for i:=0 to pred(nbr) do write(quant[i]:6:3);
        writeln;}
		sw:=0.0;
		for i:=0 to pred(nbr) do sw:=sw+pob[i];
		for i:=0 to pred(nbr) do pob[i]:=pob[i]/sw;
		//for i:=0 to pred(nbr) do write(pob[i]:6:3);writeln;
        Dkl:=0.0;Dabs:=0.0;Deuc:=0.0;Dqua:=0.0;Dcra:=0.0;
        if tout then for j:=0 to pred(nbr) do begin pobth[k][j]:=pob[j];quanth[k][j]:=quant[j];end else
          begin
            for i:=0 to pred(nbr) do if pobth[k][i]*pob[i]>0.0 then 
              begin
				x:=anbr*ln(pob[i]/pobth[k][i]);
				Dkl:=Dkl+0.5*(pob[i]*x-x*pobth[k][i]);
              end;
            for i:=0 to pred(nbr) do Dabs:=Dabs+abs(pob[i]-pobth[k][i]);Dabs:=anbr*Dabs;
            for i:=0 to pred(nbr) do Deuc:=Deuc+sqr(pob[i]-pobth[k][i]);Deuc:=anbr*sqrt(Deuc);
            for i:=0 to pred(nbr) do Dqua:=Dqua+sqr(quant[i]-quanth[k][i]);Dqua:=anbr*sqrt(Dqua);
			combrank2(he,ht[k],rangx,rangy);
			N:=length(he);M:=length(ht[k]);
			U:=0.0;for i:=0 to high(he) do U:=U+sqr(rangx[i]-succ(i));U:=U*N;
			V:=0.0;for i:=0 to high(ht[k]) do V:=V+sqr(rangy[i]-succ(i));V:=V*M;
			Dcra:=(U+V)/(N*M*(N+M)) - (4*M*N-1)/6/(M+N);Dcra:=Dcra/M;
          end;
        hetmoy[k]:=sx[k]/nl;
        hetvar[k]:=(sx2[k]-sqr(sx[k])/nl)/nl;
        //write('hetmoy[',k,']=',hetmoy[k]:6:4,'     hetvar[',k,']=',hetvar[k]:12:6);
        a[k]:=(hetmoy[k]*(1.0-hetmoy[k]))/hetvar[k]-1.0; 
        b[k]:=(1.0-hetmoy[k])*a[k];
        a[k]:=hetmoy[k]*a[k];
        //writeln('     a[',k,']=',a[k]:8:4,'   b[',k,']=',b[k]:8:4,'  dist=',sqrt(sqr(a[k]-ah[k])+sqr(b[k]-bh[k])):8:4,'  Dkl=',Dkl:10:6);
        if (not tout) then 
          begin
            write(f2,sqrt(sqr(a[k]-ah[k])+sqr(b[k]-bh[k])):10:8,'     ',Dkl:10:8,'     ',Dabs:10:8,'     ',Deuc:10:8,'     ',Dqua:10:8,'     ',Dcra:10:8);
            writeln(f2,'   H',k);
            DD[0,0,k,rep]:=Dkl;DD[0,1,k,rep]:=Dabs;DD[0,2,k,rep]:=Deuc;DD[0,3,k,rep]:=Dqua;DD[0,4,k,rep]:=Dcra;
          end;
      end;
  end;
  
procedure NeiDist(tout:boolean);
  var
	i,j,k,nl,ipop,jpop,np,loc,q,nlocu,N,M :integer;
	neimoy,neivar,sx,sx2,a,b,ne,rangx,rangy  :vecteurF;
	Dkl,x,Dabs,Deuc,fi,fj,gi,gj,sw,Dqua,Dcra,U,V :extended;
	locOK  :array of array of boolean;
	nei  :array of array of extended;
	pob,quant :array of extended;
  begin
    if tout then nlocu:=nloc else nlocu:=nloca;
    np:=npop*pred(npop) div 2;
    setlength(nei,np);for i:=0 to pred(np) do setlength(nei[i],nloc);setlength(ne,nlocu);
    setlength(neimoy,npop);setlength(neivar,npop);setlength(sx,np);setlength(sx2,np);setlength(a,np);setlength(b,np);
    setlength(locOK,np,nloc);
    setlength(pob,nbr);setlength(quant,nbr);
    for i:=0 to pred(np) do for loc:=0 to pred(nlocu) do locOK[i][index[loc]]:=true;
    k:=-1;
	for ipop:=0 to npop-2 do
	  begin
		for jpop:=succ(ipop) to pred(npop) do
		  begin
			inc(k);
			sx[k]:=0.0;sx2[k]:=0.0;nl:=0;for i:=0 to pred(nbr) do pob[i]:=0.0;
			for loc:=0 to pred(nlocu) do
			  begin
			    if (samplesize[ipop][index[loc]]>40)and(samplesize[jpop][index[loc]]>40) then
				  begin
					locOK[k][index[loc]]:=true;inc(nl);
					fi:=freq[ipop][index[loc]];fj:=freq[jpop][index[loc]];gi:=1.0-fi;gj:=1.0-fj;
					nei[k][index[loc]]:=(fi*fj+gi*gj)/sqrt(sqr(fi)+sqr(gi))/sqrt(sqr(fj)+sqr(gj));
				  end else locOK[k][index[loc]]:=false;
			  end;	
			for loc:=0 to pred(nlocu) do if locOK[k][index[loc]] then 
			  begin 
				i:=trunc(nbr*nei[k][index[loc]]);
				if (i>pred(nbr)) then i:=pred(nbr);
				pob[i]:=pob[i]+1.0;
			  end;
			sw:=0.0;for i:=0 to pred(nbr) do sw:=sw+pob[i];
			for i:=0 to pred(nbr) do pob[i]:=pob[i]/sw;
			for loc:=0 to pred(nlocu) do ne[loc]:=nei[k][index[loc]];  
			if tout then begin setlength(nt[k],nlocu);for loc:=0 to pred(nlocu) do nt[k][loc]:=nei[k][index[loc]];end;
			fsort('a',nlocu,ne);
			for i:=0 to pred(nbr) do begin q:= succ(i)*nlocu div succ(nbr);quant[i]:=ne[q];end;
			{if tout then write('TOUT ');
			write('Nei (',k,')   ');
			for i:=0 to pred(nbr) do write(quant[i]:6:3);
			writeln;}
			Dkl:=0.0;Dabs:=0.0;Deuc:=0.0;Dqua:=0.0;
			if tout then for j:=0 to pred(nbr) do begin pobtn[k][j]:=pob[j];quantn[k][j]:=quant[j];end else
			  begin
				for i:=0 to pred(nbr) do if pobtn[k][i]*pob[i]>0.0 then 
				  begin
					x:=anbr*ln(pob[i]/pobtn[k][i]);
					Dkl:=Dkl+0.5*(pob[i]*x-x*pobtn[k][i]);
				  end;
				for i:=0 to pred(nbr) do Dabs:=Dabs+abs(pob[i]-pobtn[k][i]);Dabs:=anbr*Dabs;
				for i:=0 to pred(nbr) do Deuc:=Deuc+sqr(pob[i]-pobtn[k][i]);Deuc:=anbr*sqrt(Deuc);
				for i:=0 to pred(nbr) do Dqua:=Dqua+sqr(quant[i]-quantn[k][i]);Dqua:=anbr*sqrt(Dqua);
				combrank2(ne,nt[k],rangx,rangy);
				N:=length(ne);M:=length(nt[k]);
				U:=0.0;for i:=0 to high(ne) do U:=U+sqr(rangx[i]-succ(i));U:=U*N;
				V:=0.0;for i:=0 to high(nt[k]) do V:=V+sqr(rangy[i]-succ(i));V:=V*M;
				Dcra:=(U+V)/(N*M*(N+M)) - (4*M*N-1)/6/(M+N);Dcra:=Dcra/M;
			  end;
		  for loc:=0 to pred(nlocu) do if locOK[k][index[loc]] then begin sx[k]:=sx[k]+nei[k][index[loc]];sx2[k]:=sx2[k]+sqr(nei[k][index[loc]]);end;
		  neimoy[k]:=sx[k]/nl;
		  neivar[k]:=(sx2[k]-sqr(sx[k])/nl)/nl;
		  //write('neimoy[',k,']=',neimoy[k]:6:4,'     neivar[',k,']=',neivar[k]:12:6);
		  a[k]:=(neimoy[k]*(1.0-neimoy[k]))/neivar[k]-1.0; 
		  b[k]:=(1.0-neimoy[k])*a[k];
		  a[k]:=neimoy[k]*a[k];
		  //writeln('     a[',k,']=',a[k]:8:4,'   b[',k,']=',b[k]:8:4,'  dist=',sqrt(sqr(a[k]-ah[k])+sqr(b[k]-bh[k])):8:4,'  Dabs=',Dabs:10:6);
		  if (not tout) then 
			begin
			  write(f2,sqrt(sqr(a[k]-ah[k])+sqr(b[k]-bh[k])):10:8,'     ',Dkl:10:8,'     ',Dabs:10:8,'     ',Deuc:10:8,'     ',Dqua:10:8,'     ',Dcra:10:8);
			  writeln(f2,'   N',k);
              DD[1,0,k,rep]:=Dkl;DD[1,1,k,rep]:=Dabs;DD[1,2,k,rep]:=Deuc;DD[1,3,k,rep]:=Dqua;DD[1,4,k,rep]:=Dcra;
			end;
		end;
    end;
end;

procedure calFst(tout:boolean);
  var
	i,k,nl,ipop,jpop,np,loc,all,q,nlocu,N,M :integer;
	fstmoy,fstvar,sx,sx2,a,b,fs,rangx,rangy  :vecteurF;
	sniA,sniAA,sni,sni2,s2A,fi,fj,sw,sw2,a1,b1,Dkl,x,Dabs,Deuc,Dqua,Dcra,U,V :extended;
	locOK  :array of array of boolean;
	fst,w  :array of array of extended;
	s1l,s3l,nc,MSI,MSP,s2I,s2P:extended;
	pob,quant :array of extended;
  begin
    if tout then nlocu:=nloc else nlocu:=nloca;
    np:=npop*pred(npop) div 2;
    setlength(fst,np);for i:=0 to pred(np) do setlength(fst[i],nloc);
    setlength(w,np);for i:=0 to pred(np) do setlength(w[i],nloc);
    setlength(fstmoy,npop);setlength(fstvar,npop);setlength(sx,np);setlength(sx2,np);setlength(a,np);setlength(b,np);
    setlength(locOK,np,nloc);
    setlength(pob,nbr);setlength(fs,nlocu);setlength(quant,nbr);
    for i:=0 to pred(np) do for loc:=0 to pred(nlocu) do locOK[i][index[loc]]:=true;
    k:=-1;
	for ipop:=0 to npop-2 do
	  begin
		for jpop:=succ(ipop) to pred(npop) do
		  begin
			inc(k);
			sx[k]:=0.0;sx2[k]:=0.0;nl:=0;for i:=0 to pred(nbr) do pob[i]:=0.0;
			for loc:=0 to pred(nlocu) do
			  begin
			    if (samplesize[ipop][index[loc]]>40)and(samplesize[jpop][index[loc]]>40) then
				  begin
					locOK[k][index[loc]]:=true;inc(nl);
					s1l:=0.0;s3l:=0.0;
					for all:=0 to 1 do
					  begin
					    if all=0 then 
						  begin fi:=1.0-freq[ipop][index[loc]];fj:=1.0-freq[jpop][index[loc]];end 
						  else
						  begin fi:=freq[ipop][index[loc]];fj:=freq[jpop][index[loc]];end;
						sniAA:=fi*samplesize[ipop][index[loc]]+fj*samplesize[jpop][index[loc]];
						sniA:=2.0*sniAA;
						sni:=1.0*(samplesize[ipop][index[loc]]+samplesize[jpop][index[loc]]);
						sni2:=1.0*(sqr(samplesize[ipop][index[loc]])+sqr(samplesize[jpop][index[loc]]));
						s2A:=2.0*samplesize[ipop][index[loc]]*sqr(fi)+2.0*samplesize[jpop][index[loc]]*sqr(fj);
						nc:=1.0*(sni-sni2/sni);
						MSI:=(0.5*sniA+sniAA-s2A)/(sni-2);
						MSP:=(s2A-0.5*sqr(sniA)/sni);
						s2I:=0.5*MSI;
						s2P:=(MSP-MSI)/(2*nc);
						s1l:=s1l+s2P;
						s3l:=s3l+s2P+s2I;
					  end;
					if (s3l>0.0)and(s1l>0.0) then fst[k][index[loc]]:=s1l/s3l else fst[k][index[loc]]:=0.0;
					w[k][index[loc]]:=1.0*nc;
				  end else locOK[k][index[loc]]:=false;
			  end;
			//  writeln('FST fin de la premiere boucle sur les locus');
			for loc:=0 to pred(nlocu) do fs[loc]:=fst[k][index[loc]];
			if tout then begin setlength(ft[k],nlocu);for loc:=0 to pred(nlocu) do ft[k][loc]:=fst[k][index[loc]];end;
			//writeln('2');
			fsort('a',nlocu,fs);
			//writeln('3');
			for i:=0 to pred(nbr) do begin q:= succ(i)*nlocu div succ(nbr);quant[i]:=fs[q];end;
			//writeln('4');
			{if tout then write('TOUT ');
			write('Fst (',k,')   ');
			for i:=0 to pred(nbr) do write(quant[i]:6:3);
			writeln;}
			  
			sw:=0.0;
			for loc:=0 to pred(nlocu) do if locOK[k][index[loc]] then sw:=sw+w[k][index[loc]];
			for loc:=0 to pred(nlocu) do if locOK[k][index[loc]] then w[k][index[loc]]:=w[k][index[loc]]/sw;
			
			for loc:=0 to pred(nlocu) do if locOK[k][index[loc]] then 
			  begin 
				i:=trunc(nbr*fst[k][index[loc]]);
				if (i>pred(nbr)) then i:=pred(nbr);
				pob[i]:=pob[i]+w[k][index[loc]];
			  end;
			//for i:=0 to pred(nbr)  do write(pob[i]:8:4);writeln;
			sw:=0.0;
			for i:=0 to pred(nbr) do sw:=sw+pob[i];
			//writeln('somme des pob=',sw:6:3);
			sw2:=0.0;for loc:=0 to pred(nlocu) do if locOK[k][index[loc]] then sw2:=sw2+sqr(w[k][index[loc]]);
			for loc:=0 to pred(nlocu) do if locOK[k][index[loc]] then begin sx[k]:=sx[k]+w[k][index[loc]]*fst[k][index[loc]];sx2[k]:=sx2[k]+w[k][index[loc]]*sqr(fst[k][index[loc]]);end;
			fstmoy[k]:=sx[k];
			fstvar[k]:=(sx2[k]-sqr(sx[k]))/(1.0-sw2);
			a[k]:=(fstmoy[k]*(1.0-fstmoy[k]))/fstvar[k]-1.0;
			b[k]:=(1.0-fstmoy[k])*a[k];
			a[k]:=fstmoy[k]*a[k];
			a1:=a[k]-1.0;b1:=b[k]-1.0;
			Dkl:=0.0;Dabs:=0.0;Deuc:=0.0;Dqua:=0.0;
			if tout then 
			  begin 
				for i:=0 to pred(nbr) do 
				  begin
					pobtf[k][i]:=pob[i];
					quantf[k][i]:=quant[i];
				  end;
			  end
			  else
			  begin
				for i:=0 to pred(nbr) do 
				  begin
					//writeln(pobtf[k][i]:12:6,pob[i]:12:6);
					if pobtf[k][i]*pob[i]>0.0 then 
					  begin
						x:=anbr*ln(pob[i]/pobtf[k][i]);
						Dkl:=Dkl+0.5*(pob[i]*x-x*pobtf[k][i]);
					  end;
				  end;
				for i:=0 to pred(nbr) do Dabs:=Dabs+abs(pob[i]-pobtf[k][i]);Dabs:=anbr*Dabs;
				for i:=0 to pred(nbr) do Deuc:=Deuc+sqr(pob[i]-pobtf[k][i]);Deuc:=anbr*sqrt(Deuc);
				for i:=0 to pred(nbr) do Dqua:=Dqua+sqr(quant[i]-quantf[k][i]);Dqua:=anbr*sqrt(Dqua);
				combrank2(fs,ft[k],rangx,rangy);
				N:=length(fs);M:=length(ft[k]);
				U:=0.0;for i:=0 to high(fs) do U:=U+sqr(rangx[i]-succ(i));U:=U*N;
				V:=0.0;for i:=0 to high(ft[k]) do V:=V+sqr(rangy[i]-succ(i));V:=V*M;
				Dcra:=(U+V)/(N*M*(N+M)) - (4*M*N-1)/6/(M+N);Dcra:=Dcra/M;
			  end;
			//write('fstmoy[',k,']=',fstmoy[k]:6:4,'     fstvar[',k,']=',fstvar[k]:12:6);
			//writeln('     a[',k,']=',a[k]:8:4,'   b[',k,']=',b[k]:8:4,'  dist=',sqrt(sqr(a[k]-af[k])+sqr(b[k]-bf[k])):8:4,'  Dkl=',Dkl:10:6);
			if (not tout) then 
			  begin
			    write(f2,sqrt(sqr(a[k]-af[k])+sqr(b[k]-bf[k])):10:8,'     ',Dkl:10:8,'     ',Dabs:10:8,'     ',Deuc:10:8,'     ',Dqua:10:8);
			    writeln(f2,'   F',k);
                DD[2,0,k,rep]:=Dkl;DD[2,1,k,rep]:=Dabs;DD[2,2,k,rep]:=Deuc;DD[2,3,k,rep]:=Dqua;DD[2,4,k,rep]:=Dcra;
			  end;
		end;
      end;
  end;
  
procedure compdist;
  var
    i,j,k,l :integer;
    sx,sx2,v,m :extended;
  begin
    writeln(nloca,' locus');writeln(f3,nloca,' locus');
	for i:=0 to 2 do //SS
	  begin
	    writeln;
	    if i=0 then begin writeln('hétérozygotie');writeln(f3,'hétérozygotie');end;
	    if i=1 then begin  writeln('Neis distance');writeln(f3,'Neis distance');end;
	    if i=2 then begin writeln('Fst distance'); writeln(f3,'Fst distance');end;
	    writeln('         Kullback                Dabs                   Deuc                 Dqua                Dcra');
	    writeln(f3,'         Kullback                Dabs                   Deuc                 Dqua                Dcra');
	    for k:=0 to 2 do //np
	      begin
	        for j:=0 to 4 do //dist
	          begin
				sx:=0.0;sx2:=0.0;
				for l:=0 to pred(nrep) do begin sx:=sx+DD[i,j,k,l];sx2:=sx2+sqr(DD[i,j,k,l]);end;
				v:=(sx2-sx*sx/nrep)/pred(nrep);m:=sx/rep;
				write(m:12:6,sqrt(v)/m:10:7);write(f3,m:12:6,sqrt(v)/m:10:7);
			  end;
			  writeln;writeln(f3);
	      end;
	  end;
	writeln;writeln;writeln(f3);writeln(f3);
  end;
  
begin
  randomize;
  t0:=now;
  readfile;
  if paramCount=1 then val(paramstr(1),nloca,io) else nloca:=nloc;
  writeln('durée de la lecture: ',TimeToStr(now-t0));
  setlength(wl,nloc);for rep:=0 to pred(nloc) do wl[rep]:=1.0;
  setlength(index,nloc);for rep:=0 to pred(nloc) do index[rep]:=rep;
  {t0:=now;
  writeln('avant ecribin');
  ecribin;
  t0:=now;
  libin;
  DecodeTime(now-t0, myHour, myMin, mySec, myMilli);
  writeln('durée de la lecture du bin: ',IntToStr(myMilli),' millisecondes');
  halt;}
  calfreq;
  DecodeTime(now-t0, myHour, myMin, mySec, myMilli);
  writeln('durée du calcul des fréquences: ',IntToStr(myMilli),' millisecondes'); 
  t0:=now;
  calhet(true);
  DecodeTime(now-t0, myHour, myMin, mySec, myMilli);
  writeln('durée du calcul des hétérozygoties: ',IntToStr(myMilli),' millisecondes');
  t0:=now;
  NeiDist(true);
  DecodeTime(now-t0, myHour, myMin, mySec, myMilli);
  writeln('durée du calcul des Nei: ',IntToStr(myMilli),' millisecondes');
  t0:=now;
  calFst(true);
  DecodeTime(now-t0, myHour, myMin, mySec, myMilli);
  writeln('durée du calcul des Fst: ',IntToStr(myMilli),' millisecondes');
  repeat Randinit(mwcg[1000],16777216,random(16777216),random(16777216),GM,randinitOK);until randinitOK;
  assignfile(f3,'snp2.out');rewrite(f3);
  for iter:=0 to 4 do
    begin
	 assignfile(f2,'comp_dist2.txt');rewrite(f2);
     nloca:=ml[iter];
	  writeln('tirage de ',nloca,' locus au hasard avec remise parmi ',nloc);
	  for rep:=0 to pred(nrep) do
		begin
		  resample(wl,nloca,index,freqmax,GM);
		  calhet(false);
		  NeiDist(false);
		  calFst(false);
		  write(chr(13),'Progression des calculs ',succ(rep)*100 div nrep,'%');
		end;
	  closefile(f2);
	  compdist;
	end;
  closefile(f3);
end.
  
class Componente {
  private:
    int id_componente;
    String nome;
    String descricao;
    int modo_operacao;
    int nivel;
    int nivel_atual;
    int nivel_min;
    int status_rele;
    int ativar;

  public:
    Componente(int id, String nome, String descricao, int modo_operacao, int nivel, int nivel_atual, int status_rele, int nivel_min) {
      this->id_componente = id;
      this->nome = nome;
      this->descricao = descricao;
      this->modo_operacao = modo_operacao;
      this->nivel = nivel;
      this->nivel_atual = nivel_atual;
      this->status_rele = status_rele;
      this->nivel_min = nivel_min;
    }

   
    void setAtivar(int _ativar){
      this->ativar = _ativar;
    }

    int getAtivar(){
      return this->ativar;
    }

    int getIdComponente() {
      return this->id_componente;
    }

    String getNome() {
      return this->nome;
    }

    String getDescricao() {
      return this->descricao;
    }

    int getModoOperacao() {
      return this->modo_operacao;
    }

    int getNivel() {
      return this->nivel;
    }

    int getNivelAtual() {
      return this->nivel_atual;
    }

    int getNivelMin() {
      return this->nivel_min;
    }

    int getStatusRele() {
      return this->status_rele;
    }
};

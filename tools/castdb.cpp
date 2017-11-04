int main(int ac, char *av[]) {
  CtrlClaw claw;
  ParrotWord parrot("./db/ostrich");
  if (parrot.loadMaster() == false) {
    return 0;
  }
}

# GB - Gustavo Wingert, Melissa Kunst e Thalia Danieli Schwaab

Nesse trabalhado, temos as seguintes funções:
- Ler mais de um OBJ: 
    - Já triangularizado, com normais e coordenadas de texturas; 
    - Cada objeto ou grupo1 (submesh) possui seu índice de textura e suas propriedades do material (ka, ks e kd) - que devem ser usados no shader de iluminação; 
- Ter pelo menos 1 fonte de luz, iluminação de Phong:  
    - Acrescentar os coeficientes ka, ks e kd do arquivo de materiais (.mtl); 
    - Preferencialmente, deixar todos os coeficientes de iluminação (intensidade das fontes de luz, coeficiente da especular);

Gif dos arquivos dos objetos:
![gif](/CGCCHibrido-main/CGCCHibrido-main/TarefaGB/images/GB-obj_e_mtl.gif)

- Controle de câmera por teclado e/ou mouse, provendo navegabilidade para a cena; 
- Seleção dos objetos via teclado e/ou mouse, permitindo a aplicação de operações geométricas (rotação, translação e escala uniforme);  

Gif do funcionamentos dos objetos e da camera:
![gif](/CGCCHibrido-main/CGCCHibrido-main/TarefaGB/images/GB-objetos_e_camera.gif)


- A especificação dos objetos passa a ser por meio do arquivo de configuração de cena, contendo: 
    - Definições sobre os .objs a serem instanciados na cena 
        - Nome do arquivo 
        - Transformações iniciais (rot, trans, escala) 
        - Em alguns objetos, alguma predefinição de animação (trajetória com curvas paramétricas) 
    - Informação da(s) fonte(s) de luz 
    - Posição e orientação inicial da câmera, assim como a definição do frustrum.  

Gif das configurações de objetos e camera:
![gif](/CGCCHibrido-main/CGCCHibrido-main/TarefaGB/images/GB-configs.gif)


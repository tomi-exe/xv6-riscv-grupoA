EVIDENCIA T1 - bundle y logs

Archivo: T1_evidencia_para_profesor.tgz
Contiene: ANEXO-commit-history-local.txt, ANEXO-reflog-tomi-exe_t1.txt, ANEXO-tomi-exe_t1.bundle, ANEXO-format-patch/, ANEXO-bundle-sha256.txt, etc.

Checksum (SHA256) en archivo T1_evidencia_para_profesor.sha256.

Instrucciones para el ayudante:
1) Descargar T1_evidencia_para_profesor.tgz
2) Verificar integridad: sha256sum T1_evidencia_para_profesor.tgz
3) Extraer: tar -xzf T1_evidencia_para_profesor.tgz
4) Clonar bundle: git clone ANEXO-tomi-exe_t1.bundle repo_from_bundle --branch tomi-exe_t1
5) Revisar commits: cd repo_from_bundle && git log --pretty=fuller --date=iso

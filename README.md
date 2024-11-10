Description du Projet

Dans les environnements urbains modernes, une gestion efficace du trafic est essentielle. Ce projet propose un système de feux de circulation intelligent qui optimise le flux de circulation en utilisant la détection en temps réel des véhicules. Il s'appuie sur des modules ESP32-CAM pour capturer des images aux intersections, un modèle YOLO pour la détection et le comptage des véhicules, et ThingSpeak pour la communication des données et des commandes.


Objectifs

Développer un système adaptatif pour les feux de circulation basé sur les conditions de trafic en temps réel.

Capturer et traiter des images des voies de circulation pour identifier et compter les véhicules.

Utiliser ThingSpeak pour envoyer les données de trafic et optimiser la durée des feux verts en fonction du nombre de véhicules.

Architecture Technique

Matériel : ESP32-CAM pour la capture d'images, LEDs pour simuler les feux de circulation, et un ordinateur pour le traitement des données.

Logiciel et Plateformes :

YOLO pour la détection des véhicules.

ThingSpeak pour la communication en cloud entre l’ESP32 et l’ordinateur.

Jupyter Notebook et Arduino IDE pour le traitement des données et le développement du firmware.

Conclusion

Ce projet vise à améliorer la fluidité du trafic en ajustant dynamiquement les cycles des feux en fonction des conditions de circulation, réduisant ainsi les embouteillages et améliorant l’expérience des usagers de la route.


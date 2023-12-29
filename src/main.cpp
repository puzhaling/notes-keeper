
#include <filesystem>

#include "./headers/notesContainer.h"
#include "./headers/common.h"

using namespace std::literals::string_literals;

const std::string path{ "../notes/" }; // EXTERN ( SEE IN COMMON.H )
std::vector<std::string> file_names{}; // EXTERN ( SEE IN COMMON.H )

inline std::fstream input{};
inline NotesContainer notes_container{};

int 
main() {
	
	for (const auto& entry : std::filesystem::directory_iterator(path)) {
		input.open(entry.path());
		file_names.emplace_back(static_cast<std::string>(entry.path()));
		notes_container.add_note(input);
		input.close();

		input.open(entry.path(), std::ios::trunc);
		input.close();
	}

	print_programm_name();

	while (true) {
		std::size_t note_idx{};
		std::size_t task_idx{};

		MAIN_MENU:

		std::cout << "\nWHAT DO YOU WANT? (-_- ;)\n";
		std::cout <<   "-----------------\n";
		std::cout << "0) print notes list.\n";
		std::cout << "1) create new note.\n";
		std::cout << "2) exit.\n";

		std::cout << "Enter number: ";
		std::uint8_t answer{ static_cast<std::uint8_t>(get_ranged_input_inclusive(0, 2)) };
		std::cout << '\n';

		switch (answer) {
		case 0:

			NOTES_LIST_MENU:

			notes_container.print_notes_list();

			while (true) {

				std::cout << "\nWHAT DO YOU WANT? (-_- ;)\n";
				std::cout <<   "-----------------\n";
				std::cout << "0) print note's content.\n";
				std::cout << "1) delete note.\n";
				std::cout << "2) exit.\n";

				std::cout << "Enter number: ";
				answer = get_ranged_input_inclusive(0, 2);

				switch (answer) {
				case 0:
					std::cout << "Enter note's number: ";

					note_idx = get_ranged_input_inclusive(0, notes_container.size() - 1);
					std::cout << '\n';
					std::cout << notes_container[note_idx];
					std::cout << '\n';

					std::cout << "\nWHAT DO YOU WANT? (-_- ;)\n";
					std::cout <<   "-----------------\n";
					std::cout << "0) change note's status.\n";
					std::cout << "1) exit.\n";

					std::cout << "Enter number: ";
					answer = get_ranged_input_inclusive(0, 1);

					switch (answer) {
					case 0:
						if (notes_container.get_note_type(notes_container[note_idx]) == "taskslist") {

							NotesContainer::TasksList* target_note{ dynamic_cast<NotesContainer::TasksList*>(notes_container[note_idx]) };

							std::cout << "Enter task number to change it's status: ";
							task_idx = get_ranged_input_inclusive(0, target_note->size() - 1);
							std::cout << '\n';

							std::vector<NotesContainer::TasksList::Task>& target_note_container{ 
								target_note->get_tasks_container() 
							};

							target_note_container[task_idx].is_completed = !target_note_container[task_idx].is_completed;
							std::cout << "Task's status was successfully changed. :)\n\n";
							std::cout << notes_container[note_idx];
							std::cout << '\n';

							// traverse through task list note to ensure that all the tasks are completed and we can change status of the whole note
							bool tasks_list_is_fully_completed{ true };
							for (const auto& task : target_note_container) {
								if (!task.is_completed) { 
									tasks_list_is_fully_completed = false;
									break;
								}
							}

							auto status{ notes_container[note_idx]->get_status() };

							if (status == NotesContainer::BasicNote::incompleted) {
								if (tasks_list_is_fully_completed)
									notes_container[note_idx]->change_status();
							}
							else if (status == NotesContainer::BasicNote::completed) {
								if (!tasks_list_is_fully_completed)
									notes_container[note_idx]->change_status();
							}
						}
						else if (notes_container.get_note_type(notes_container[note_idx]) == "document") {
							notes_container[note_idx]->change_status();
							std::cout << "\nDocument's status was successfully changed. :)\n\n";

							notes_container.print_notes_list();
						}
						break;
					case 1:
						std::cout << '\n';
						goto NOTES_LIST_MENU;
					} 
					break;
				case 1:
					std::cout << "Enter note's number to delete it: ";
					answer = get_ranged_input_inclusive(0, notes_container.size() - 1);
					notes_container.delete_note(answer);

					std::cout << "\n\nDeletion was successfully completed :(\n";

					[[fallthrough]];
				case 2:
					goto MAIN_MENU;
				}
			}
			break;
		case 1:
			notes_container.create_note();
			goto MAIN_MENU;
		case 2:
			goto PROGRAMM_END;
		}

	}

	PROGRAMM_END:

	auto file_names_iter{ file_names.begin() };
	for (const auto& note : notes_container) {
		input.open(*file_names_iter);
		input << note;
		input.close();
		file_names_iter++;
	}

	return EXIT_SUCCESS;
}
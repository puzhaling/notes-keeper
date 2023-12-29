
#include "./headers/helpers.h"
#include "./headers/notesContainer.h"
#include "./headers/common.h"

void
NotesContainer::BasicNote::set_status(std::string_view status) {
	if (status == "|incompleted|") {
		m_status = incompleted;
	} else {
		m_status = completed;
	} 
}

std::istream&
operator>>(std::istream& in, NotesContainer::BasicNote* note) {
	if (NotesContainer::get_note_type(note) == "taskslist") {

		while (!in.eof()) {
			NotesContainer::TasksList* tasks_list{ dynamic_cast<NotesContainer::TasksList*>(note) };

			NotesContainer::TasksList::Task new_task{};

			std::string text{};
			std::string status{};

			std::getline(in >> std::ws, text, '(');
			remove_trailing_whitespaces(text);
			new_task.text = text;

			in >> std::ws >> status;
			status = '(' + status;

			new_task.is_completed = ((status == "(+)") ? true : false);

			tasks_list->m_tasks.push_back(new_task);
		}

	} else if (NotesContainer::get_note_type(note) == "document") {

		NotesContainer::Document* document{ dynamic_cast<NotesContainer::Document*>(note) };

		for (std::string line; std::getline(in >> std::ws, line);)
			document->m_text.append(line), document->m_text.push_back(' ');
		remove_trailing_whitespaces(document->m_text);

	}

	return in;
}

std::ostream& 
operator<<(std::ostream& out, NotesContainer::BasicNote* note) {
	if (NotesContainer::get_note_type(note) == "document") {
		out << "#document " << note->get_name() << " |" << NotesContainer::BasicNote::status_to_str(note->get_status()) << "| \n\n";

		NotesContainer::Document* document{ dynamic_cast<NotesContainer::Document*>(note) };
		std::size_t len{ document->size() };

		auto counter{ NotesContainer::Document::get_output_width() };
		for (std::size_t i{}; i < len; ++i) {
			if (counter) {
				out << document->m_text[i];
				--counter;
			}
			else {
				while (!std::isspace(document->m_text[i]))
					out << document->m_text[i++];
				out << '\n';
				counter = NotesContainer::Document::get_output_width();
			}
		}
	}
	else if (NotesContainer::get_note_type(note) == "taskslist") {
		out << "#tasks " << note->get_name();

		NotesContainer::TasksList* tasks_list{ dynamic_cast<NotesContainer::TasksList*>(note) };

		std::vector<NotesContainer::TasksList::Task>& tasks_container{ tasks_list->get_tasks_container() };

		bool tasks_list_is_fully_completed{ true };
		for (const auto& task : tasks_container) {
			if (!task.is_completed) {
				tasks_list_is_fully_completed = !tasks_list_is_fully_completed;
				break;
			}
		}

		out << " |";
		switch (tasks_list_is_fully_completed) {
		case true: out << NotesContainer::BasicNote::status_to_str(NotesContainer::BasicNote::completed); break;
		case false: out << NotesContainer::BasicNote::status_to_str(NotesContainer::BasicNote::incompleted); break;
		}
		out << "| \n\n";

		auto start{ tasks_container.begin() };
		for (;start != tasks_container.end() - 1; ++start) 
			out << start->text << ' ' << (start->is_completed ? "(+)" : "(-)") << '\n';
		out << start->text << ' ' << (start->is_completed ? "(+)" : "(-)");
	}

	return out;
}

NotesContainer&
NotesContainer::add_note(std::istream& in) {

	std::string type{};
	in >> std::ws >> type;

	std::string name{};
	std::getline(in >> std::ws, name, '|');
	remove_trailing_whitespaces(name);

	std::string status{};
	std::getline(in >> std::ws, status, '\n');
	status = '|' + status;
	remove_trailing_whitespaces(status);
	
	if (type == "#tasks") {

		TasksList* new_tasks_list{ new TasksList() };
		new_tasks_list->set_name(name);
		new_tasks_list->set_status(status);
		in >> new_tasks_list;
		this->m_notes.emplace_back(new_tasks_list);

	} else if (type == "#document") {

		Document* new_document{ new Document() };
		new_document->set_name(name);
		new_document->set_status(status);
		in >> new_document;
		this->m_notes.emplace_back(new_document);

	}

	return *this;
}

void
NotesContainer::print_notes_list() const {
	std::uint16_t counter{ 0 };

	std::cout << "NOTES LIST:\n";
	std::cout << "-----------\n";
	for (auto* note : m_notes) {
		std::cout << counter++ << ") " << std::quoted(note->get_name());

		std::cout << " status: (" << NotesContainer::BasicNote::status_to_str(note->get_status()) << ") ";

		if (get_note_type(note) == "taskslist") {
			std::cout << "type : (taskslist)";
		} else if (get_note_type(note) == "document") {
			std::cout << "type : (document)";
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}

std::string_view
NotesContainer::get_note_type(BasicNote* note) {
	if (dynamic_cast<Document*>(note) != nullptr) return "document"sv;
	return "taskslist"sv;
}

void
NotesContainer::create_note() {
	std::cout << "What type of note do you want to create?\n";
	std::cout << "0) document.\n";
	std::cout << "1) taskslist.\n";

	std::cout << "Enter number: ";
	std::uint8_t decision{ static_cast<std::uint8_t>(get_ranged_input_inclusive(0, 1)) };

	if (decision == 0) {

		Document* new_document{ new Document() };

		std::cout << "\nEnter new document name: ";
		std::string name{};
		std::getline(std::cin >> std::ws, name);

		std::cout << "\nEnter text:\n";
		std::string text{};
		std::getline(std::cin >> std::ws, text);

		remove_trailing_whitespaces(text);

		new_document->set_name(name);
		new_document->set_text(text);
		this->m_notes.push_back(new_document);

		std::ofstream new_file_output{ path + name + ".txt" };
		file_names.push_back(path + name + ".txt");

	} else if (decision == 1) {

		TasksList* new_tasks_list{ new TasksList() };

		std::cout << "\nEnter new taskslist name: ";
		std::string name{};
		std::getline(std::cin >> std::ws, name);

		new_tasks_list->set_name(name);
		this->m_notes.emplace_back(new_tasks_list);

		while (true) {
			std::cout << "\nWhat do you want to do?\n";
			std::cout << "0) add task.\n";
			std::cout << "1) exit.\n";

			std::cout << "Enter number: ";
			std::uint8_t decision{ static_cast<std::uint8_t>(get_ranged_input_inclusive(0, 1)) };

			if (decision == 0) {
				TasksList::Task new_task{};
				std::cout << "Enter new task description: ";
				std::getline(std::cin >> std::ws, new_task.text);
				remove_trailing_whitespaces(new_task.text);

				new_tasks_list->get_tasks_container().emplace_back(new_task);
			} else {
				break;
			}
		}

		std::ofstream new_file_output{ path + name + ".txt" };
		file_names.push_back(path + name + ".txt");
	}
}

void
NotesContainer::delete_note(std::size_t idx) {
	std::string name{ m_notes[idx]->get_name() };

	auto file_iter{ find(std::begin(file_names), std::end(file_names), name) };
	file_names.erase(file_iter);

	auto note_iter{ find_if(std::begin(m_notes), std::end(m_notes), 
		[&](const auto* note) -> bool { return note->get_name() == name; }) 
	};

	delete(*note_iter);
	m_notes.erase(note_iter);

	remove(std::string(path + name + ".txt").c_str());
}